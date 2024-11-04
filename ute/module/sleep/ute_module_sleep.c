/**
 *@file
 *@brief        ˯��
 *@details
 *@author       zn.zeng
 *@date       2021-10-25
 *@version      v1.0
 */

#include "ute_module_sleep.h"
#include "ute_module_log.h"
#include "ute_module_heart.h"
#include "ute_module_filesystem.h"
#include "ute_application_common.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_flash_map_common.h"
#include "ute_module_sleep.h"

/*! ˯���㷨����zn.zeng, 2021-10-26  */
ute_sleep_data_t uteSleepAlgoData;
ute_module_sleep_data_t uteModuleSleepData;
/* ˯�߻����� zn.zeng 2022-02-14*/
void *uteModuleSleepMute;

/**
 *@brief       ˯�߳�ʼ��
 *@details
 *@author        zn.zeng
 *@date        2021-10-26
 */
void uteModuleSleepInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_SLEEP_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SLEEP_DIR, NULL, NULL);
#endif
    uteModulePlatformCreateMutex(&uteModuleSleepMute);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCWMSleepInit();
#endif
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    memset(&uteModuleSleepData, 0, sizeof(ute_module_sleep_data_t));
    memset(&uteSleepAlgoData, 0, sizeof(ute_sleep_data_t));
    uteSleepAlgoData.maxRecord = UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE;
    uteSleepAlgoData.saveData.recordIndex = 0;
    uteSleepAlgoData.saveData.year = time.year;
    uteSleepAlgoData.saveData.mon = time.month;
    uteSleepAlgoData.saveData.day = time.day;
    uteSleepOnBandSetPoint(&uteSleepAlgoData);
    void *file;
    uint8_t path[20];
    memset(&path[0], 0, 20);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_SLEEP_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read file=%s", __func__, &path[0]);
    uint32_t currTimeMinSec = time.hour*3600+time.min*60+time.sec;
    bool isNotGetUp = false;
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))  //��ȡ����
    {
        if ((currTimeMinSec >= (18 * 3600)) || (currTimeMinSec < (6 * 3600))) //>=18:00  <6:00 // ����״̬
        {
            isNotGetUp = true;
            UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,not get up", __func__);
        }
        else// �Ѿ�����״̬
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &(uteSleepAlgoData.saveData), sizeof(ute_sleep_data_save_t));
            if (uteSleepAlgoData.saveData.recordIndex != 0)
            {
                uteSleepAlgoData.isGetUp = true;
            }
            else
            {
                uteSleepAlgoData.isGetUp = false;
            }
        }
        uteModuleFilesystemCloseFile(file);
    }
#if UTE_OPTIMIZE_SLEEP_SUPPORT
    else
    {
        if ((currTimeMinSec >= (18 * 3600)) || (currTimeMinSec < (6 * 3600))) //>=18:00  <6:00 // ����״̬
        {
            isNotGetUp = true;
            UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,not get up", __func__);
        }
    }
#endif
    if (isNotGetUp)
    {
        // if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SLEEP_TMP_DATA, &file, FS_O_RDONLY))
        // {
        //     uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        //     uteModuleFilesystemReadData(file, &uteSleepAlgoData, sizeof(ute_sleep_data_t));
        //     uteModuleFilesystemCloseFile(file);
        //     UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read tmp data", __func__);
        // }
        uteSleepAlgoData.isGetUp = false;
    }
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
    uteSleepAlgoData.isGetUp = false;
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCwmReadNapSleepDataFromFs(time);
#endif
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currTimeMinSec=%d,uteSleepAlgoData.isGetUp=%d", __func__,currTimeMinSec,uteSleepAlgoData.isGetUp);
}
/**
 *@brief       ˯�����ݱ���
 *@details
 *@author        zn.zeng
 *@date        2021-10-26
 */
void uteModuleSleepSaveSleepData(ute_module_systemtime_time_t time)
{
    /*! û�����ӹ�APP���������� xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = sizeof(ute_sleep_data_save_t);
    uint8_t path[20];
    memset(&path[0], 0, 20);
    uint16_t currTimeHourMin = time.hour << 8 | time.min;
    /*! 18~24�㲻�������� zn.zeng 2022-03-22*/
    if (currTimeHourMin > 0x1200)
    {
        return;
    }
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
#if 0 // test data
    static uint8_t testDay = 0;
    // time.day = time.day+testDay;
    time.day = 26;
    uteSleepAlgoData.saveData.year = time.year;
    uteSleepAlgoData.saveData.mon = time.month;
    uteSleepAlgoData.saveData.day = time.day;
    for(int k=0; k<50; k++)
    {
        uteSleepAlgoData.saveData.sleep_record[k].startTime.hour = testDay;
        uteSleepAlgoData.saveData.sleep_record[k].startTime.min = testDay;
        uteSleepAlgoData.saveData.sleep_record[k].sleepFlag = rand()%3;
        uteSleepAlgoData.saveData.sleep_record[k].period = k;
        uteSleepAlgoData.saveData.sleep_record[k].state = 4;
    }
    if (testDay % 2)
    {
        uteSleepAlgoData.saveData.recordIndex = 25;
    }
    else
    {
        uteSleepAlgoData.saveData.recordIndex = 50;
    }
    testDay++;
#endif // test data
    sprintf((char *)&path[0], "%04d%02d%02d", time.year, time.month, time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SLEEP_DIR, dirInfo, NULL);
    if ((dirInfo->filesCnt >= UTE_MODULE_SLEEP_DATA_MAX_DAYS) && (memcmp(&path[0], &dirInfo->filesName[0][0], 8) != 0))
    {
        /*! ɾ�����һ�������zn.zeng, 2021-10-26*/
        memset(&path[0], 0, 20);
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_SLEEP_DIR, &dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0], 0, 20);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_SLEEP_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,dirInfo->filesCnt=%d", __func__, dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,save file=%s", __func__, &path[0]);
    /*! ���浱ǰ����zn.zeng, 2021-10-26  */
    void *file;
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &(uteSleepAlgoData.saveData), buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,open file err", __func__);
    }
    uteModulePlatformMemoryFree(dirInfo);
}
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
/**
 *@brief       ͨ����ȡС˯˯����������ͼ
 *@details  ��ʱ�����ڶ�ȡ
 *@return true Ϊ��������
 *@author        zn.zeng
 *@date        2021-10-26
 */
bool uteModuleSleepReadDayData(uint16_t year, uint8_t mon, uint8_t day, ute_sleep_data_save_t *pread)
{
    bool result = false;
    uint8_t dataIndx = 0;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,saveNapData.recordIndex=%d", __func__, uteCwmSleepData.saveNapData.recordIndex);
    if ((uteCwmSleepData.saveNapData.recordIndex > 0))  //(systemTime == readTime) &&
    {
        bool sleepStart = false;
        uint16_t firstStartMin = 0;
        uint8_t startIdx = 0;
        for(uint8_t i=0; i<uteCwmSleepData.saveNapData.recordIndex; i++)
        {
            if(uteCwmSleepData.saveNapData.sleep_record[i].state == LIGHT_SLEEP && sleepStart == false)
            {
                sleepStart = true;
                firstStartMin = uteCwmSleepData.saveNapData.sleep_record[i].startTime.hour*60+uteCwmSleepData.saveNapData.sleep_record[i].startTime.min;
                startIdx = i;
            }
            if((uteCwmSleepData.saveNapData.sleep_record[i].state == STOP_SLEEP || i==(uteCwmSleepData.saveNapData.recordIndex-1)) && sleepStart == true)
            {
                sleepStart = false;
                uint16_t currStatusStartTime =  uteCwmSleepData.saveNapData.sleep_record[i].startTime.hour*60+uteCwmSleepData.saveNapData.sleep_record[i].startTime.min;
                uint16_t diffMin = (currStatusStartTime+24*60-firstStartMin)%(24*60);
                if(diffMin>=3*60)
                {
                    dataIndx += 1;
                    memset((void*)pread,0,sizeof(ute_sleep_data_save_t));
                    pread->year = uteCwmSleepData.saveNapData.year;
                    pread->mon = uteCwmSleepData.saveNapData.mon;
                    pread->day = uteCwmSleepData.saveNapData.day;
                    pread->recordIndex = i - startIdx + 1;
                    memcpy(&pread->sleep_record[0],&uteCwmSleepData.saveNapData.sleep_record[startIdx],sizeof(sleep_record_t)*pread->recordIndex);
                    pread->getUpTime.hour = currStatusStartTime / 60;
                    if (pread->getUpTime.hour > 24)
                    {
                        pread->getUpTime.hour = pread->getUpTime.hour - 24;
                    }
                    pread->getUpTime.min = currStatusStartTime % 60;
                }
            }
        }
    }

    if(dataIndx>=1)
    {
        result = true;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.dataIndx=%d", __func__, dataIndx);

    return result;
}
#else
/**
 *@brief       ˯�����ݶ�ȡ
 *@details  ��ʱ�����ڶ�ȡ
 *@return true Ϊ��������
 *@author        zn.zeng
 *@date        2021-10-26
 */
bool uteModuleSleepReadDayData(uint16_t year, uint8_t mon, uint8_t day, ute_sleep_data_save_t *pread)
{
    uint32_t currTime = year << 16 | mon << 8 | day;
    uint32_t readTime = 0;
    uint16_t dayMin;
    uint16_t size = sizeof(ute_sleep_data_save_t);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint32_t systemTime = time.year << 16 | time.month << 8 | time.day;
    pread->recordIndex = 0;
    if (currTime == systemTime)
    {
        dayMin = time.hour * 60 + time.min;
        if ((dayMin > 1080) || (dayMin < 300)) // 18:00~5:00
        {
            return false;
        }
    }
    readTime = uteSleepAlgoData.saveData.year << 16 | uteSleepAlgoData.saveData.mon << 8 | uteSleepAlgoData.saveData.day;
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read buff currTime=%x,readTime=%x", __func__, currTime, readTime);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,uteSleepAlgoData.saveData.recordIndex = %d", __func__, uteSleepAlgoData.saveData.recordIndex);
    if ((currTime == readTime) && (uteSleepAlgoData.saveData.recordIndex > 0))
    {
        uteModulePlatformTakeMutex(uteModuleSleepMute);
        memcpy(pread, &uteSleepAlgoData.saveData, size);
        if ((!uteSleepAlgoData.isGetUp) && (uteSleepAlgoData.saveData.recordIndex < UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE))
        {
            if (uteSleepAlgoData.saveData.sleep_record[uteSleepAlgoData.saveData.recordIndex].period > 0)
            {
                pread->recordIndex = pread->recordIndex + 1;
            }
        }

        dayMin = pread->sleep_record[pread->recordIndex - 1].startTime.hour * 60 + pread->sleep_record[pread->recordIndex - 1].startTime.min + pread->sleep_record[pread->recordIndex - 1].period;
        pread->getUpTime.hour = dayMin / 60;
        if (pread->getUpTime.hour > 24)
        {
            pread->getUpTime.hour = pread->getUpTime.hour - 24;
        }
        pread->getUpTime.min = dayMin % 60;
        uteModulePlatformGiveMutex(uteModuleSleepMute);
        return true;
    }
    void *file;
    uint8_t path[20];
    memset(&path[0], 0, 20);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_SLEEP_DIR, year, mon, day);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read file=%s", __func__, &path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, pread, size);
        uteModuleFilesystemCloseFile(file);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,pread->recordIndex=%d", __func__, pread->recordIndex);
        if (pread->recordIndex > 0)
        {
            dayMin = pread->sleep_record[pread->recordIndex - 1].startTime.hour * 60 + pread->sleep_record[pread->recordIndex - 1].startTime.min + pread->sleep_record[pread->recordIndex - 1].period;
            pread->getUpTime.hour = dayMin / 60;
            if (pread->getUpTime.hour > 24)
            {
                pread->getUpTime.hour = pread->getUpTime.hour - 24;
            }
            pread->getUpTime.min = dayMin % 60;
            return true;
        }
    }
    pread->recordIndex = 0;
    return false;
}
#endif

#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
/**
 *@brief       ˯�����ݽ���
 *@details    ÿ����һ�Σ���sport ����
 *@author        zn.zeng
 *@date        2021-10-26
 */
void uteModuleSleepSaveSampleData(uint16_t minStepCnt, uint8_t minSleepCnt, ute_module_systemtime_time_t time, uint8_t avgHeart)
{
    /*! û�����ӹ�APP���������� xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint8_t tmp[8];
    tmp[0] = (minStepCnt >> 8) & 0xff;
    tmp[1] = minStepCnt & 0xff;
    tmp[2] = time.month;
    tmp[3] = time.day;
    tmp[4] = time.hour;
    tmp[5] = time.min;
    tmp[6] = minSleepCnt;
    tmp[7] = avgHeart;
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA, &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
        int32_t size = uteModuleFilesystemGetFileSize(file);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,size=%d", __func__, size);
        if (size >= (11520*2)) //����5(57600)�����ݣ�ɾ���������¿�ʼ����
        {
            UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,seek set", __func__);
            uteModuleFilesystemCloseFile(file);
            uteModuleFilesystemDelFile(UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA);
            uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA, &file, FS_O_WRONLY | FS_O_CREAT);
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        }
        uteModuleFilesystemWriteData(file, &tmp[0], 8);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
 *@brief        ��ʼ����˯�߲�������
 *@details
 *@author       zn.zeng
 *@date       2021-12-29
 */
void uteModuleSleepSendSleepSampleData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t sendBuff[247];
    uint8_t cpySize = 0;
    uint8_t sendSize = 0;
    memset(&sendBuff[0], 0, 247);
    sendBuff[0] = CMD_SLEEP_ON_BAND;
    sendBuff[1] = 0x03;
    uint16_t mtu = uteApplicationCommonGetMtuSize();
    cpySize = ((mtu - 2) / 8) * 8;
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA, &file, FS_O_RDONLY))
    {
        if (sendParam->currSendFileSize == 0)
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
            sendParam->currSendFileSize = uteModuleFilesystemGetFileSize(file);
        }
        if ((sendParam->currSendFileDataOffset + cpySize) > sendParam->currSendFileSize)
        {
            cpySize = sendParam->currSendFileSize - sendParam->currSendFileDataOffset;
        }
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,cpySize=%d,currSendFileDataOffset=%d,currSendFileSize=%d", __func__, cpySize, sendParam->currSendFileDataOffset, sendParam->currSendFileSize);
        uteModuleFilesystemSeek(file, sendParam->currSendFileDataOffset, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &sendBuff[2], cpySize);
        uteModuleFilesystemCloseFile(file);
        sendSize = cpySize + 2;
        sendParam->currSendFileDataOffset = sendParam->currSendFileDataOffset + cpySize;
        uteModuleProfileBle50SendToPhone(&sendBuff[0], sendSize);
        if (sendParam->currSendFileDataOffset == sendParam->currSendFileSize)
        {
            goto SLEEP_SAMPLE_SEND_FINISH;
        }
    }
    else
    {
    SLEEP_SAMPLE_SEND_FINISH:
        uteApplicationCommonSyncDataTimerStop();
        sendBuff[1] = 0x04;
        sendSize = 2;
        uteModuleProfileBleSendToPhone(&sendBuff[0], sendSize);
    }
}

/**
 *@brief        ׼����ʼ����˯�߲�������
 *@details
 *@author       zn.zeng
 *@date       2021-12-29
 */
void uteModuleSleepStartSendSleepSampleData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->currSendFileDataOffset = 0;
    param->currSendFileSize = 0;
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSleepSendSleepSampleData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,", __func__);
}
/**
 *@brief        ����˯��debug����
 *@details     ÿСʱ����һ��debug����
 *@author       zn.zeng
 *@date       2022-03-29
 */
void uteModuleSleepSaveLogData(ute_module_systemtime_time_t time)
{
    /*! û�����ӹ�APP���������� xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    void *file;
    uint8_t path[30];
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR, dirInfo, NULL);
    if (dirInfo->filesCnt >= 24)
    {
        memset(&path[0], 0, 30);
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR, &dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0], 0, 30);
    sprintf((char *)&path[0], "%s/%04d%02d%02d%02d",UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR,time.year, time.month, time.day, time.hour);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,%s", __func__,&path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &(uteSleepAlgoData.saveData), sizeof(ute_sleep_data_save_t));
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(dirInfo);
}
/**
 *@brief        ��ʼ����˯��log����
 *@details
 *@author       zn.zeng
 *@date       2022-03-29
 */
void uteModuleSleepSendSleepLogData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t sendBuff[247];
    uint8_t cpySize = 0;
    uint8_t sendSize = 0;
    memset(&sendBuff[0], 0, 247);
    sendBuff[0] = CMD_DEBUG_TEST_DATA;
    sendBuff[1] = 0x07;
    sendBuff[2] = 0x02;
    uint16_t mtu = uteApplicationCommonGetMtuSize();
    cpySize = mtu - 3;
    void *file;
    uint8_t path[30];
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    if (sendParam->dirInfo.filesCnt > 0)
    {
        memset(&path[0], 0, 30);
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currSendFileIndex=%d,currSendFileDataOffset=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendFileDataOffset);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,mtu=%d,dataBuffSize=%d,cpySize=%d", __func__,mtu,sendParam->dataBuffSize,cpySize);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &dataBuff[0], sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        uint32_t lastSize = sendParam->dataBuffSize - sendParam->currSendFileDataOffset;
        if ( lastSize < cpySize)
        {
            cpySize = lastSize;
        }
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,lastSize=%d,cpySize=%d", __func__,lastSize,cpySize);
        memcpy(&sendBuff[3],&dataBuff[sendParam->currSendFileDataOffset], cpySize);
        sendSize = cpySize + 3;
        uteModuleProfileBle50SendToPhone(&sendBuff[0], sendSize);
        sendParam->currSendFileDataOffset = sendParam->currSendFileDataOffset + cpySize;
        if (sendParam->currSendFileDataOffset == sendParam->dataBuffSize)
        {
            sendParam->currSendFileDataOffset = 0;
            sendParam->currSendFileIndex++;
            if (sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt)
            {
                goto SEND_SLEEP_LOG_FINISH;
            }
        }
    }
    else
    {
    SEND_SLEEP_LOG_FINISH:
        sendSize = 3;
        sendBuff[2] = 0x03;
        uteApplicationCommonSyncDataTimerStop();
        uteModuleProfileBleSendToPhone(&sendBuff[0], sendSize);
    }
    uteModulePlatformMemoryFree(dataBuff);
}
/**
 *@brief        ׼����ʼ����˯��log����
 *@details
 *@author       zn.zeng
 *@date       2022-03-29
 */
void uteModuleSleepStartSendSleepLogData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    param->dataBuffSize = sizeof(ute_sleep_data_save_t);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSleepSendSleepLogData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
}
#endif
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
/**
 *@brief        ��ȡģ������
 *@details
 *@return false Ϊ��ȡ����
 *@author       zn.zeng
 *@date       2021-12-29
 */
bool uteModuleSleepReadSimulationData(ute_module_systemtime_time_t *time, ute_step_sleep_param_t *sleepData, uint8_t *heart)
{
    static uint32_t cnt = 0;
    static bool isSleepReadSimulationDataFinish = false;
    if (isSleepReadSimulationDataFinish || (cnt > 2880))
    {
        // return false;
        isSleepReadSimulationDataFinish = false;
        cnt = 0;
        uteSleepAlgoData.isGetUp = false;
    }
    uint8_t tmp[8];
    uteModulePlatformWdgFeed();
    uteModulePlatformFlashNorRead(&tmp[0], OTA_TMP_ADDR + cnt * 8, 8);
    if (tmp[0] == 0xff)
    {
        isSleepReadSimulationDataFinish = true;
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,finish cnt=%d", __func__, cnt);
        return false;
    }
    time->hour = tmp[4];
    time->min = tmp[5];
    *heart = tmp[7];
    sleepData->currentMinuteSleepTurnCnt = tmp[6];
    sleepData->currentMinuteStepCnt = tmp[0] >> 8 | tmp[1];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,cnt=%d", __func__, cnt);
    cnt++;
    return true;
}
#endif
/**
 *@brief       ˯�ߵ�����±�����ʱ���ݣ����ڿ��ػ��ָ�
 *@details    ÿ����һ�Σ���sport ����
 *@author        zn.zeng
 *@date        2022-04-12
 */
void uteModuleSleepSaveTmpDataForPowerOff(void)
{
    /*! û�����ӹ�APP���������� xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SLEEP_TMP_DATA, &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &uteSleepAlgoData, sizeof(ute_sleep_data_t));
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,", __func__);
}

/**
 *@brief       ˯�����ݽ���
 *@details    ÿ����һ�Σ���sport ����
 *@author        zn.zeng
 *@date        2021-10-26
 */
void uteModuleSleepDataInputSecond(ute_step_sleep_param_t *sleepData, ute_module_systemtime_time_t time, bool isStepping)
{
    uint8_t heart = (uint8_t)uteModuleHeartGetHeartValue();
    if ((heart != 0) && (uteModuleSleepData.heartCnt < 60))
    {
        uteModuleSleepData.oneMinHeartValue[uteModuleSleepData.heartCnt] = heart;
        uteModuleSleepData.heartCnt++;
    }
    if (time.sec == 59)
    {
        uint8_t avgHeart = 0;
        if (sleepData->currentMinuteSleepTurnCnt > 250)
        {
            sleepData->currentMinuteSleepTurnCnt = 250;
        }
        if (isStepping)
        {
            sleepData->currentMinuteSleepTurnCnt = 0xff;
        }
        if (uteModuleSleepData.heartCnt != 0)
        {
            uint32_t oneMinHeartSun = 0;
            for (uint8_t i = 0; i < uteModuleSleepData.heartCnt; i++)
            {
                oneMinHeartSun += uteModuleSleepData.oneMinHeartValue[i];
            }
            avgHeart = oneMinHeartSun / uteModuleSleepData.heartCnt;
        }
        if (!uteModuleHeartIsWear())
        {
            avgHeart = 0;
            sleepData->currentMinuteStepCnt = 0;
            sleepData->currentMinuteSleepTurnCnt = 0xfd;
        }
        if (!uteModuleHeartIsAutoTesting())
        {
            avgHeart = 40;
        }
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,simulation data start time=%d", __func__, uteModulePlatformGetSystemTick());
        static bool isSimulationFinish = false;
        if (isSimulationFinish)
        {
            goto SIMULATION_FINISH;
        }
        while (uteModuleSleepReadSimulationData(&time, sleepData, &avgHeart))//һ����ģ��
            // if(uteModuleSleepReadSimulationData(&time, sleepData, &avgHeart))//��ʵģ�⣬ÿ������һ��
        {
#endif
            uint32_t currTimeSec = time.hour*3600+time.min*60+time.sec;
#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
            uteModuleSleepSaveSampleData(sleepData->currentMinuteStepCnt, sleepData->currentMinuteSleepTurnCnt, time, avgHeart);
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
            if (currTimeSec == (18*3600-1))
            {
                uteModuleSleepSaveSleepData(time);
                uteSleepOnBandClearSleepStatus();
                uteSleepAlgoData.saveData.recordIndex = 0;
                uteSleepAlgoData.isGetUp = false;
                uteModuleSleepData.isCurrHasSleepData = false;
            }
#else
            UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currTimeSec=%d,.isGetUp=%d", __func__,currTimeSec,uteSleepAlgoData.isGetUp);
            UTE_MODULE_LOG_BUFF(UTE_LOG_SLEEP_LVL,(uint8_t *)&(uteSleepAlgoData.saveData),sizeof(ute_sleep_data_save_t));
            if (!uteSleepAlgoData.isGetUp)
            {
                UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,avgHeart=%d,recordIndex=%d,.heartCnt=%d", __func__, avgHeart, uteSleepAlgoData.saveData.recordIndex, uteModuleSleepData.heartCnt);
                uteModulePlatformTakeMutex(uteModuleSleepMute);
                uteSleepAnalysisOnBand(time.hour, time.min, sleepData->currentMinuteSleepTurnCnt, sleepData->currentMinuteStepCnt, avgHeart, 0x01);
                uteSleepAlgoData.saveData.recordIndex = uteSleepOnBandGetRecordIndex();
                UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currentMinuteSleepTurnCnt=%d,currentMinuteStepCnt=%d,.isStepping=%d,.isGetUp=%d", __func__, sleepData->currentMinuteSleepTurnCnt, sleepData->currentMinuteStepCnt, isStepping, uteSleepAlgoData.isGetUp);
                if (uteSleepAlgoData.isGetUp)
                {
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
                    for (uint8_t i = 0; i < 50; i++)
                    {
                        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,%02d:%02d state=%d,period=%d", __func__, uteSleepAlgoData.saveData.sleep_record[i].startTime.hour, uteSleepAlgoData.saveData.sleep_record[i].startTime.min, uteSleepAlgoData.saveData.sleep_record[i].state, uteSleepAlgoData.saveData.sleep_record[i].period);
                    }
#endif
                    if (currTimeSec < (18*3600-1))//<18:00
                    {
                        if(uteSleepAlgoData.saveData.recordIndex>0)
                        {
                            uteModuleSleepData.isCurrHasSleepData = true;
                        }
                        uteModuleSleepSaveSleepData(time);
                        uteSleepOnBandClearSleepStatus();
                        uteSleepAlgoData.saveData.recordIndex = 0;
                    }
                }
                else
                {
                    if (currTimeSec == (18*3600-1))
                    {
                        uteModuleSleepSaveSleepData(time);
                        uteSleepOnBandClearSleepStatus();
                        uteSleepAlgoData.saveData.recordIndex = 0;
                        uteSleepAlgoData.isGetUp = false;
                        uteModuleSleepData.isCurrHasSleepData = false;
                    }
                }
                // uteModuleSleepSaveTmpDataForPowerOff();
                uteModulePlatformGiveMutex(uteModuleSleepMute);
            }
            else
            {
#if UTE_OPTIMIZE_SLEEP_SUPPORT
                if (currTimeSec >= (18*3600)||(currTimeSec < (6*3600))) //add by pcm 20220812 ��18~6�����ʱ����ڣ����isGetUpΪtrue���򶼽�����������һ�Σ�Ȼ�����˯������
#else
                if (currTimeSec == (18*3600-1))  //delete by pcm 20220812
#endif
                {
                    uteSleepOnBandClearSleepStatus();
                    uteSleepAlgoData.saveData.recordIndex = 0;
                    uteSleepAlgoData.isGetUp = false;
                    uteModuleSleepData.isCurrHasSleepData = false;
                }
            }
#endif
            uteSleepAlgoData.saveData.year = time.year;
            uteSleepAlgoData.saveData.mon = time.month;
            uteSleepAlgoData.saveData.day = time.day;
#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
            if(time.min == 59)
            {
                uteModuleSleepSaveLogData(time);
            }
#endif
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
        }
        // else
        // {
        //     isSimulationFinish = true;//��ʵģ�⣬ÿ������һ��
        // }
    SIMULATION_FINISH:
        isSimulationFinish = true; //һ����ģ��
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isSimulationFinish=%d,simulation data end time=%d", __func__,isSimulationFinish,uteModulePlatformGetSystemTick());
#endif
        sleepData->currentMinuteSleepTurnCnt = 0;
        sleepData->currentMinuteStepCnt = 0;
        uteModuleSleepData.heartCnt = 0;
    }
}

/**
 *@brief        ��ʼ����˯������
 *@details
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepSendSleepHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[20];
    uint8_t response[247];
    uint16_t sendSize = 0;
    uint16_t mtuSize = uteApplicationCommonGetMtuSize();
    sendParam->dataBuffSize = sizeof(ute_sleep_data_save_t);
    ute_sleep_data_save_t *pRead = (ute_sleep_data_save_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset((void*)pRead, 0, sendParam->dataBuffSize);
    memset(&path[0], 0, 20);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__, sendParam->currSendFileIndex, sendParam->currSendHourIndex, sendParam->dirInfo.filesCnt);

    if (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt)
    {
        void *file;
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_SLEEP_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read file=%s", __func__, &path[0]);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,(void*)pRead, sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        if (sendParam->currSendHourIndex == 0)
        {
            response[0] = CMD_SLEEP_ON_BAND;
            response[1] = 0x01;
            response[2] = (pRead->year >> 8) & 0xff;
            response[3] = pRead->year & 0xff;
            response[4] = pRead->mon;
            response[5] = pRead->day;
            response[6] = pRead->recordIndex;
            sendSize = 7;
            uteModuleProfileBleSendToPhone(&response[0], sendSize);
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,pRead->recordIndex=%d,currSendHourIndex=%d", __func__, pRead->recordIndex, sendParam->currSendHourIndex);
    if (sendParam->currSendHourIndex < pRead->recordIndex)
    {
        response[0] = CMD_SEND_SLEEP_ON_BAND_DATAS;
        sendSize = 1;
        for (; sendParam->currSendHourIndex < pRead->recordIndex; sendParam->currSendHourIndex++)
        {
            if ((sendSize + 6) > mtuSize)
            {
                break;
            }
            response[sendSize] = pRead->sleep_record[sendParam->currSendHourIndex].startTime.hour;
            response[sendSize + 1] = pRead->sleep_record[sendParam->currSendHourIndex].startTime.min;
            response[sendSize + 2] = pRead->sleep_record[sendParam->currSendHourIndex].state;
            response[sendSize + 3] = pRead->sleep_record[sendParam->currSendHourIndex].sleepFlag;
            response[sendSize + 4] = pRead->sleep_record[sendParam->currSendHourIndex].period >> 8 & 0xff;
            response[sendSize + 5] = pRead->sleep_record[sendParam->currSendHourIndex].period & 0xff;
            sendSize += 6;
        }
        uteModuleProfileBle50SendToPhone(&response[0], sendSize);
    }
    if (sendParam->currSendHourIndex == pRead->recordIndex)
    {
        if (sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt)
        {
            uteApplicationCommonSyncDataTimerStop();
            response[0] = CMD_SLEEP_ON_BAND;
            response[1] = 0x02;
            sendSize = 2;
            uteModuleProfileBleSendToPhone(&response[0], sendSize);
        }
        else
        {
            sendParam->currSendHourIndex = 0;
            sendParam->currSendFileIndex++;
        }
    }
    uteModulePlatformMemoryFree((void*)pRead);
}

/**
 *@brief        ׼����ʼ����˯������
 *@details
 *@param[in]  ��Ҫͬ��������ʱ���
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepStartSendSleepHistoryData(ute_module_systemtime_time_t time)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    if (!uteSleepAlgoData.isGetUp)
    {
        if (uteSleepAlgoData.saveData.recordIndex < UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
        {
            if (uteSleepAlgoData.saveData.sleep_record[uteSleepAlgoData.saveData.recordIndex].period > 0)
            {
                uteSleepAlgoData.saveData.recordIndex = uteSleepAlgoData.saveData.recordIndex + 1;
            }
        }
        ute_module_systemtime_time_t readTime;
        uteModuleSystemtimeGetTime(&readTime);
        uteModuleSleepSaveSleepData(readTime);
    }
    param->crc = 0;
    param->time = time;
    if (param->time.year == 0)
    {
        param->isAllData = true;
    }
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SLEEP_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSleepSendSleepHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,", __func__);
}

/**
 *@brief        ��ȡ��ǰ˯������
 *@details
 *@param[out]  ute_module_sleep_display_data_t *sleepDisplayData ,����ָ��
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepGetCurrDayDataDisplay(ute_module_sleep_display_data_t *sleepDisplayData)
{
    sleepDisplayData->lightSleepMin = 0;
    sleepDisplayData->deepSleepMin = 0;
    sleepDisplayData->wakeSleepMin = 0;
    sleepDisplayData->totalSleepMin = 0;
#if UTE_REM_SLEEP_SUPPORT
    sleepDisplayData->RemSleepMin = 0;
#endif
    sleepDisplayData->SleepMin = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    ute_sleep_data_save_t *pRead = (ute_sleep_data_save_t *)uteModulePlatformMemoryAlloc(sizeof(ute_sleep_data_save_t));
    memset((void*)pRead,0,sizeof(ute_sleep_data_save_t));
    if (uteModuleSleepReadDayData(time.year, time.month, time.day, pRead))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,recordIndex=%d", __func__, pRead->recordIndex);
        for (int k = 0; k < pRead->recordIndex; k++)
        {
            if (pRead->sleep_record[k].state == LIGHT_SLEEP)
            {
                sleepDisplayData->lightSleepMin += pRead->sleep_record[k].period;
            }
            else if (pRead->sleep_record[k].state == DEEP_SLEEP)
            {
                sleepDisplayData->deepSleepMin += pRead->sleep_record[k].period;
            }
            else if (pRead->sleep_record[k].state == AWAKE_SLEEP)
            {
                sleepDisplayData->wakeSleepMin += pRead->sleep_record[k].period;
            }
#if UTE_REM_SLEEP_SUPPORT
            else if (pRead->sleep_record[k].state == REM_SLEEP)
            {
                sleepDisplayData->RemSleepMin += pRead->sleep_record[k].period;
            }
#endif
        }

#if UTE_REM_SLEEP_SUPPORT
        sleepDisplayData->SleepMin = sleepDisplayData->lightSleepMin + sleepDisplayData->deepSleepMin;
#if UTE_MODULE_SLEEP_NOT_AWAKE_DATA_SUPPORT
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->RemSleepMin;
#else
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->wakeSleepMin+sleepDisplayData->RemSleepMin;
#endif
        sleepDisplayData->fallAsSleepTime.hour = pRead->sleep_record[0].startTime.hour;
        sleepDisplayData->fallAsSleepTime.min = pRead->sleep_record[0].startTime.min;
        sleepDisplayData->getUpSleepTime.hour = pRead->getUpTime.hour;
        sleepDisplayData->getUpSleepTime.min = pRead->getUpTime.min;
#else
        sleepDisplayData->SleepMin = sleepDisplayData->lightSleepMin + sleepDisplayData->deepSleepMin;
#if UTE_MODULE_SLEEP_NOT_AWAKE_DATA_SUPPORT
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin;
#else
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->wakeSleepMin;
#endif
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->wakeSleepMin;
        sleepDisplayData->fallAsSleepTime.hour = pRead->sleep_record[0].startTime.hour;
        sleepDisplayData->fallAsSleepTime.min = pRead->sleep_record[0].startTime.min;
        sleepDisplayData->getUpSleepTime.hour = pRead->getUpTime.hour;
        sleepDisplayData->getUpSleepTime.min = pRead->getUpTime.min;
#endif
    }
    else
    {
        memset(&sleepDisplayData->fallAsSleepTime, 0x00, sizeof(timestmap_t));
        memset(&sleepDisplayData->getUpSleepTime, 0x00, sizeof(timestmap_t));
    }
    //uteModuleSleepSetCurrDayDataGraphs(pRead,sleepDisplayData);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,deepSleepMin=%d,lightSleepMin=%d,wakeSleepMin=%d", __func__, sleepDisplayData->deepSleepMin, sleepDisplayData->lightSleepMin, sleepDisplayData->wakeSleepMin);
    uteModulePlatformMemoryFree(pRead);
}
/**
 *@brief        ʱ��仯
 *@details   ����������18:00ʱҪ��������
 *@param[in] ute_module_systemtime_time_t curr,��ǰʱ��
 *@param[in] ute_module_systemtime_time_t newSet,��Ҫ���õ�ʱ��
 *@author       zn.zeng
 *@date       2022-04-20
 */
void uteModuleSleepSystemtimeChange(ute_module_systemtime_time_t curr,ute_module_systemtime_time_t newSet)
{
    uint32_t newTime = newSet.year<<16|newSet.month<<8|newSet.day;
    uint32_t currTime = curr.year<<16|curr.month<<8|curr.day;
    bool isNeedSave = false;
    if (!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    if(newTime!=currTime)
    {
        isNeedSave = true;
    }
    else
    {
        uint16_t newTimeHourMin = newSet.hour << 8 | newSet.min;
        uint16_t currTimeHourMin = curr.hour << 8 | curr.min;
        // 18:00 ->0x1200
        if ((currTimeHourMin < 0x1200) && (newTimeHourMin >= 0x1200))
        {
            isNeedSave = true;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,isNeedSave=%d", __func__,isNeedSave);
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    if(isNeedSave)
    {
        if (!uteSleepAlgoData.isGetUp)
        {
            uteModuleSleepSaveSleepData(curr);
        }
        uteSleepOnBandClearSleepStatus();
        uteSleepAlgoData.isGetUp = false;
        uteSleepAlgoData.saveData.recordIndex = 0;
    }
#endif
}

/**
 *@brief        ��ȡ�����𴲱�־
 *@details
 *@author       zn.zeng
 *@date       2022-04-20
 */
bool uteModuleSleepCurrDayIsGetUp(void)
{
    return uteSleepAlgoData.isGetUp;
}
/**
 *@brief        �Ƿ���˯��
 *@details
 *@author       zn.zeng
 *@date       2022-04-20
 */
bool uteModuleSleepCurrDayIsHasSleep(void)
{
    return uteModuleSleepData.isCurrHasSleepData;
}

#if UTE_MODULE_CYWEE_MOTION_SUPPORT
/*
 *��ά˯���㷨�������
 *
 */
static uint16_t lastSleepStatusStartTime = 0;  //��һ��״̬��ʼʱ��
static uint16_t notifySleepStatusTime = 0;  //֪ͨ˯�ߵ�ʱ��,����˯����ʼʱ��
/*
isSleep true:��ʾ˯�߽��� false:��ʾС˯����
*/
void uteModuleSleepAnalysis(uteCwmSleepOnceData_t uteCwmSleepOnceData,bool isSleep)
{
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL,"isSleep = %d uteCwmSleepOnceData.sleepStatus = %d",isSleep,uteCwmSleepOnceData.sleepStatus);

    if(isSleep == true)
    {
        if(uteModuleCwmIsSleeping() == false &&  uteCwmSleepOnceData.sleepStatus != CWM_SLEEP_STATUS_SLEEP_NOTIFICATION)
        {
            return;
        }
    }
    else
    {
        if(uteModuleCwmIsNaping()==false &&  uteCwmSleepOnceData.sleepStatus != 0)
        {
            return;
        }
    }
    ute_sleep_data_save_t *pTempSleepData = NULL;

    pTempSleepData = &uteCwmSleepData.saveNapData;

    uteCwmSleepData.sleepStatus = uteCwmSleepOnceData.sleepStatus;
    uint8_t saveIndex = pTempSleepData->recordIndex;
    uint16_t diffMin = 0;

    switch(uteCwmSleepOnceData.sleepStatus)
    {
        case CWM_SLEEP_STATUS_FALL_SLEEP:   //��˯����ǳ˯����
        case CWM_SLEEP_STATUS_LIGHT_SLEEP:
        {
            if(uteCwmSleepOnceData.sleepStatus == CWM_SLEEP_STATUS_FALL_SLEEP)
            {
                notifySleepStatusTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                lastSleepStatusStartTime = notifySleepStatusTime;
                /*��һ��˯���������3Сʱ,�µ���˯ʱ���������С�ڵ���30����,������״̬����*/
                if(uteCwmSleepData.lastSleepIsLongTime == true)
                {
                    if(saveIndex > 0)
                    {
                        uint16_t lastSleepStopTime = pTempSleepData->sleep_record[saveIndex - 1].startTime.hour*60+pTempSleepData->sleep_record[saveIndex - 1].startTime.min;
                        diffMin = (notifySleepStatusTime+24*60-lastSleepStopTime)%(24*60);
                        if(diffMin<=30 && pTempSleepData->sleep_record[saveIndex - 1].state == STOP_SLEEP)
                        {

                            // pTempSleepData->sleep_record[saveIndex - 1].startTime.min = uteCwmSleepOnceData.min;
                            pTempSleepData->sleep_record[saveIndex - 1].state = AWAKE_SLEEP;
                            pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
                            lastSleepStatusStartTime = lastSleepStopTime;
                        }
                    }
                    uteCwmSleepData.lastSleepIsLongTime = false;
                }
            }
            if(saveIndex >= UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
            {
                break;
            }
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[saveIndex].state = LIGHT_SLEEP;
            pTempSleepData->sleep_record[saveIndex].period = 0;
            if(saveIndex > 0 )
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
            }
            lastSleepStatusStartTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
            pTempSleepData->recordIndex++;
            if(isSleep == false)
            {
                uteModuleCwmIsSetNaping(true);
            }
        }
        break;
        case CWM_SLEEP_STATUS_DEEP_SLEEP:
        {
            if(saveIndex >= UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
            {
                break;
            }
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[saveIndex].state = DEEP_SLEEP;
            pTempSleepData->sleep_record[saveIndex].period = 0;
            if(saveIndex > 0 )
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
            }
            lastSleepStatusStartTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
            pTempSleepData->recordIndex++;
        }
        break;
        case CWM_SLEEP_STATUS_AWAKE:
        {
            if(saveIndex >= UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
            {
                break;
            }
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[saveIndex].state = AWAKE_SLEEP;
            pTempSleepData->sleep_record[saveIndex].period = 0;
            if(saveIndex > 0 )
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
            }
            lastSleepStatusStartTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
            pTempSleepData->recordIndex++;
        }
        break;
        case CWM_SLEEP_STATUS_REM:
        {
            if(saveIndex >= UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
            {
                break;
            }
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[saveIndex].state = REM_SLEEP;
            pTempSleepData->sleep_record[saveIndex].period = 0;
            if(saveIndex > 0 )
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
            }
            lastSleepStatusStartTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
            pTempSleepData->recordIndex++;
        }
        break;
        case CWM_SLEEP_STATUS_SLEEP_NOTIFICATION:
        {
            uteModuleCwmIsSetSleeping(true);
        }
        break;
        case CWM_SLEEP_STATUS_CLOSE_NAP_SLEEP: //����С˯
        case CWM_SLEEP_STATUS_STOP_SLEEP:
        {
            uint8_t saveIndex = pTempSleepData->recordIndex;
            uint16_t diffTotalMin = 0;
            ute_module_systemtime_time_t time;
            uteModuleSystemtimeGetTime(&time);
            uint32_t currTimeSec = time.hour*3600+time.min*60+time.sec;
            if(uteCwmSleepOnceData.sleepStatus == CWM_SLEEP_STATUS_STOP_SLEEP)
            {
                uteModuleCwmIsSetSleeping(false);
            }
            else
            {
                uteModuleCwmIsSetNaping(false);
            }
            uint16_t currTimeHourMin = time.hour << 8 | time.min;
            if(saveIndex > 0)
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
                diffTotalMin = (currStatusStartTime+24*60-notifySleepStatusTime)%(24*60);
            }
#if UTE_MODULE_RUNING_LOG_SUPPORT
            UTE_MODULE_LOG_BLE(UTE_LOG_CYWEE_MOTION_LVL, "lastStatusDiffMin = %d",diffMin);
#endif
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[pTempSleepData->recordIndex].state = STOP_SLEEP;
            pTempSleepData->recordIndex++;
            uteModuleSleepSaveNapSleepData();
            if(diffTotalMin>=3*60) //������ǳ�˯��
            {
                uteCwmSleepData.lastSleepIsLongTime = true;
            }
            /*! 18~24�㲻��ҹ��˯��*/
            bool saveFlag = true;
            if (currTimeHourMin > 0x1200)
            {
                saveFlag = false;
            }
            if(diffTotalMin>=3*60 && saveFlag && uteSleepAlgoData.isGetUp == false) //����3Сʱ˯�� & ��ǰʱ�䲻����18:00
            {
                uteSleepAlgoData.isGetUp = true;
                if (currTimeSec < (18*3600-1))//<18:00
                {
                    if(pTempSleepData->recordIndex>0)
                    {
                        uteModuleSleepData.isCurrHasSleepData = true;
                    }
                }
            }
        }
        break;
        case CWM_SLEEP_STATUS_REQ_LAST_DATA:
        {
            //ͨ��SCL_REQ_SLEEPING_DATA ��ѯ�����һ�����ݶ�������,������˵�
            uint8_t saveIndex = pTempSleepData->recordIndex;
            if(saveIndex>0)
            {
                if(pTempSleepData->sleep_record[saveIndex-1].state == AWAKE_SLEEP)
                {
                    pTempSleepData->sleep_record[saveIndex-1].startTime.hour = 0;
                    pTempSleepData->sleep_record[saveIndex-1].startTime.min = 0;
                    pTempSleepData->sleep_record[saveIndex-1].state = INIDLE;
                    pTempSleepData->sleep_record[saveIndex-1].period = 0;
                    pTempSleepData->recordIndex --;
                    //���˲�ѯ˯�߷��ص����һ����֮��,�ڰ�ǰһ������ʱ����¡�
                    if(pTempSleepData->recordIndex > 0)
                    {
                        lastSleepStatusStartTime = pTempSleepData->sleep_record[pTempSleepData->recordIndex-1].startTime.hour * 60 + pTempSleepData->sleep_record[pTempSleepData->recordIndex-1].startTime.min;
                    }
                }
            }
        }
        break;
        default:
            break;
    }
}
/**
 *@brief    С˯˯�����ݶ�ȡ
 *@details  ��ʱ�����ڶ�ȡ
 *@return true Ϊ��������
 *@author   casen
 *@date     2022-10-18
 */
bool uteModuleSleepReadDayNapData(ute_module_sleep_nap_display_data_t *pread)
{
    uint32_t readTime = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint32_t systemTime = time.year << 16 | time.month << 8 | time.day;
    pread->recordCnt = 0;
    uint8_t dataIndx = 0;
#if (1&&UTE_MODULE_LOG_SUPPORT) //test
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 14;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 21;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = LIGHT_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex ++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 14;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 53;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = STOP_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 19;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 01;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = LIGHT_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 21;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 53;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = STOP_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 22;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 01;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = LIGHT_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 23;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 53;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = STOP_SLEEP;

#endif
    readTime = uteCwmSleepData.saveNapData.year << 16 | uteCwmSleepData.saveNapData.mon << 8 | uteCwmSleepData.saveNapData.day;
    if ((uteCwmSleepData.saveNapData.recordIndex > 0))  //(systemTime == readTime) &&
    {
        bool sleepStart = false;
        for(uint8_t i=0; i<uteCwmSleepData.saveNapData.recordIndex; i++)
        {
            if(uteCwmSleepData.saveNapData.sleep_record[i].state == LIGHT_SLEEP && sleepStart == false)
            {
                sleepStart = true;
                pread->data[dataIndx].startTime = uteCwmSleepData.saveNapData.sleep_record[i].startTime;
            }
            if(uteCwmSleepData.saveNapData.sleep_record[i].state == STOP_SLEEP && sleepStart == true)
            {
                sleepStart = false;
                pread->data[dataIndx].endTime = uteCwmSleepData.saveNapData.sleep_record[i].startTime;
                uint16_t lastStatusStartTime =  pread->data[dataIndx].startTime.hour*60+pread->data[dataIndx].startTime.min;
                uint16_t currStatusStartTime =  pread->data[dataIndx].endTime.hour*60+pread->data[dataIndx].endTime.min;
                pread->data[dataIndx].min = (currStatusStartTime+24*60-lastStatusStartTime)%(24*60);
                if(pread->data[dataIndx].min<3*60)
                {
                    dataIndx += 1;
                }
            }
            if(dataIndx>=UTE_SLEEP_NAP_DATA_DISPLAY_MAX_RECORD)
            {
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_CYWEE_MOTION_LVL, "%s,dataIndx=%d", __func__, dataIndx);
        pread->recordCnt = dataIndx;
        return true;
    }
    return false;
}

/**
 *@brief       ��ά��ʼ����˯������
 *@details
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepSendCwmNapSleepHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[32];
    uint8_t response[247];
    uint16_t sendSize = 0;
    uint16_t mtuSize = uteApplicationCommonGetMtuSize();
    sendParam->dataBuffSize = sizeof(ute_sleep_data_save_t);
    ute_sleep_data_save_t *pRead = (ute_sleep_data_save_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset((void*)pRead, 0, sendParam->dataBuffSize);
    memset(&path[0], 0, 32);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__, sendParam->currSendFileIndex, sendParam->currSendHourIndex, sendParam->dirInfo.filesCnt);

    if (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt)
    {
        void *file;
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_CWM_SLEEP_NAP_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read file=%s", __func__, &path[0]);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,(void*)pRead, sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        if (sendParam->currSendHourIndex == 0)
        {
            response[0] = CMD_CYWEE_READ_NAPSLEEP_DATA;
            response[1] = 0x01;
            response[2] = (pRead->year >> 8) & 0xff;
            response[3] = pRead->year & 0xff;
            response[4] = pRead->mon;
            response[5] = pRead->day;
            response[6] = pRead->recordIndex;
            sendSize = 7;
            uteModuleProfileBleSendToPhone(&response[0], sendSize);
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,pRead->recordIndex=%d,currSendHourIndex=%d", __func__, pRead->recordIndex, sendParam->currSendHourIndex);
    if (sendParam->currSendHourIndex < pRead->recordIndex)
    {
        response[0] = CMD_CYWEE_READ_NAPSLEEP_DATA;
        response[1] = 0x02;
        sendSize = 2;
        for (; sendParam->currSendHourIndex < pRead->recordIndex; sendParam->currSendHourIndex++)
        {
            if ((sendSize + 6) > mtuSize)
            {
                break;
            }
            response[sendSize] = pRead->sleep_record[sendParam->currSendHourIndex].startTime.hour;
            response[sendSize + 1] = pRead->sleep_record[sendParam->currSendHourIndex].startTime.min;
            response[sendSize + 2] = pRead->sleep_record[sendParam->currSendHourIndex].state;
            response[sendSize + 3] = pRead->sleep_record[sendParam->currSendHourIndex].sleepFlag;
            response[sendSize + 4] = pRead->sleep_record[sendParam->currSendHourIndex].period >> 8 & 0xff;
            response[sendSize + 5] = pRead->sleep_record[sendParam->currSendHourIndex].period & 0xff;
            sendSize += 6;
        }
        uteModuleProfileBle50SendToPhone(&response[0], sendSize);
    }
    if (sendParam->currSendHourIndex == pRead->recordIndex)
    {
        if (sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt)
        {
            uteApplicationCommonSyncDataTimerStop();
            response[0] = CMD_CYWEE_READ_NAPSLEEP_DATA;
            response[1] = 0x03;
            sendSize = 2;
            uteModuleProfileBleSendToPhone(&response[0], sendSize);
        }
        else
        {
            sendParam->currSendHourIndex = 0;
            sendParam->currSendFileIndex++;
        }
    }
    uteModulePlatformMemoryFree((void*)pRead);
}

/**
 *@brief        ׼����ʼ����˯������
 *@details
 *@param[in]  ��Ҫͬ��������ʱ���
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepStartSendCwmNapSleepHistoryData(ute_module_systemtime_time_t time)
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
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CWM_SLEEP_NAP_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSleepSendCwmNapSleepHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,", __func__);
}

#endif
