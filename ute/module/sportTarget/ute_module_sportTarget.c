/**
*@file
*@brief        运动目标设置处理模块
*@details
*@author       dengli.lu
*@date       2022-03-08
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_sportTarget.h"
#include "ute_module_protocol.h"
#include "ute_application_common.h"
#include "ute_module_sport.h"

ute_module_sport_target_data_t uteModuleSportTargetData;
#if APP_MODULE_SCREENS_TARGET_MORE_SUPPORT
ute_module_sport_target_set_data_t uteModuleSportTargetMoreData[UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM];
#endif

/**
*@brief  初始化函数
*@details
*@author        dengli.lu
*@date        2022-03-08
*/
void uteModuleSportTargetInit(void)
{
    uteModuleSportTargetData.isSportDistanceTargetNotify = false;
    uteModuleSportTargetData.isSportDistanceTargetHasNotify = false;
    uteModuleSportTargetData.isSportTimeTargetNotify = false;
    uteModuleSportTargetData.isSportTimeTargetHasNotify = false;
    uteModuleSportTargetData.isSportKcalTargetNotify = false;
    uteModuleSportTargetData.isSportKcalTargetHasNotify = false;
    uteModuleSportTargetData.isSportOneHourNotify = false;
    uteModuleSportTargetData.isSportOneHourHasNotify = false;
    uteModuleSportTargetData.isSportOneKmNotify = false;
    uteModuleSportTargetData.isSportOneKmHasNotify = false;
#if APP_MODULE_SCREENS_TARGET_MORE_SUPPORT
    uteModuleSportTargetMoreReadConfig();
#else
    uteModuleSportTargetReadConfig();
#endif
}
/**
*@brief     读取数据
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleSportTargetReadConfig(void)
{
    /*! 保存到文件dengli.lu, 2022-04-24*/
    void *file;
    uint8_t readbuff[11];
    memset(readbuff,0,11);
    readbuff[0] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET>>24)&0xff;
    readbuff[1] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET>>16)&0xff;
    readbuff[2] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET>>8)&0xff;
    readbuff[3] = UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET&0xff;
    uint32_t timeTargetSec = UTE_MORE_SPORTS_DEFAULT_TIME_TARGET_MIN*60;
    readbuff[4] = (timeTargetSec>>24)&0xff;
    readbuff[5] = (timeTargetSec>>16)&0xff;
    readbuff[6] = (timeTargetSec>>8)&0xff;
    readbuff[7] = timeTargetSec&0xff;
    readbuff[8] = (UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET>>8)&0xff;
    readbuff[9] = UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET&0xff;
    readbuff[10] = 0;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_DATA,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],11);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleSportTargetData.sportDistanceTargeCnt = readbuff[0]<<24|readbuff[1]<<16|readbuff[2]<<8|readbuff[3];
    uteModuleSportTargetData.sportTimeTargetSec = readbuff[4]<<24|readbuff[5]<<16|readbuff[6]<<8|readbuff[7];
    uteModuleSportTargetData.sportKcalTarget = readbuff[8]<<8|readbuff[9];
    uteModuleSportTargetData.isNoTarget = readbuff[10];
}
/**
*@brief     保存数据到内存
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleSportTargetSaveData(void)
{
#if APP_MODULE_SCREENS_TARGET_MORE_SUPPORT
    uteModuleSportTargetMoreSetMoreFromIndex(uteModuleSportTargetData.showIndex - 1);
#else
    /*! 保存到文件，dengli.lu, 2022-04-24*/
    void *file;
    uint8_t writebuff[11];
    writebuff[0] = (uteModuleSportTargetData.sportDistanceTargeCnt>>24)&0xff;
    writebuff[1] = (uteModuleSportTargetData.sportDistanceTargeCnt>>16)&0xff;
    writebuff[2] = (uteModuleSportTargetData.sportDistanceTargeCnt>>8)&0xff;
    writebuff[3] = (uteModuleSportTargetData.sportDistanceTargeCnt)&0xff;
    writebuff[4] = (uteModuleSportTargetData.sportTimeTargetSec>>24)&0xff;
    writebuff[5] = (uteModuleSportTargetData.sportTimeTargetSec>>16)&0xff;
    writebuff[6] = (uteModuleSportTargetData.sportTimeTargetSec>>8)&0xff;
    writebuff[7] = (uteModuleSportTargetData.sportTimeTargetSec)&0xff;
    writebuff[8] = (uteModuleSportTargetData.sportKcalTarget>>8)&0xff;
    writebuff[9] = (uteModuleSportTargetData.sportKcalTarget)&0xff;
    writebuff[10] = uteModuleSportTargetData.isNoTarget;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_DATA,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],11);
        uteModuleFilesystemCloseFile(file);
    }
#endif
}
#if APP_MODULE_SCREENS_TARGET_MORE_SUPPORT
/**
*@brief     读取数据
*@details
*@author        wuhuowang
*@date        2023-05-08
*/
void uteModuleSportTargetMoreReadConfig(void)
{
    void *file;
    uint32_t timeTargetSec = 0;
    uint8_t readbuff[UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM * 12];
    uint8_t i = 0;
    memset(readbuff, 0, sizeof(readbuff));
    timeTargetSec = UTE_MORE_SPORTS_DEFAULT_TIME_TARGET_MIN * 60;
    for (i = 0; i < UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM; i ++)
    {
        readbuff[i * 12 + 0] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET >> 24) & 0xff;
        readbuff[i * 12 + 1] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET >> 16) & 0xff;
        readbuff[i * 12 + 2] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET >> 8) & 0xff;
        readbuff[i * 12 + 3] = UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET & 0xff;
        readbuff[i * 12 + 4] = (timeTargetSec >> 24) & 0xff;
        readbuff[i * 12 + 5] = (timeTargetSec >> 16) & 0xff;
        readbuff[i * 12 + 6] = (timeTargetSec >> 8) & 0xff;
        readbuff[i * 12 + 7] = timeTargetSec & 0xff;
        readbuff[i * 12 + 8] = (UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET >> 8) & 0xff;
        readbuff[i * 12 + 9] = UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET & 0xff;
        readbuff[i * 12 + 10] = 0;
        readbuff[i * 12 + 11] = 0;
    }
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_SET_DATA, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM * 12);
        uteModuleFilesystemCloseFile(file);
    }
    for (i = 0; i < UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM; i ++)
    {
        uteModuleSportTargetMoreData[i].sportDistanceTargeCnt = (readbuff[i * 12 + 0] << 24) | (readbuff[i * 12 + 1] << 16) | (readbuff[i * 12 + 2] << 8) | readbuff[i * 12 + 3];
        uteModuleSportTargetMoreData[i].sportTimeTargetSec = (readbuff[i * 12 + 4] << 24) | (readbuff[i * 12 + 5] << 16) | (readbuff[i * 12 + 6] << 8) | readbuff[i * 12 + 7];
        uteModuleSportTargetMoreData[i].sportKcalTarget = (readbuff[i * 12 + 8] << 8) | readbuff[i * 12 + 9];
        uteModuleSportTargetMoreData[i].isNoTarget = readbuff[i * 12 + 10];
        uteModuleSportTargetMoreData[i].showType = readbuff[i * 12 + 11];
    }
}
/**
*@brief     保存数据到内存
*@details
*@author        wuhuowang
*@date        2025-05-08
*/
void uteModuleSportTargetMoreSaveData(bool reset)
{
    void *file;
    uint8_t i = 0;
    uint8_t index = 0;
    uint32_t timeTargetSec = 0;
    uint8_t writebuff[UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM * 12];
    ute_module_sport_target_set_data_t tempSportTargetMoreData[UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM];
    memset(writebuff, 0, sizeof(writebuff));
    timeTargetSec = UTE_MORE_SPORTS_DEFAULT_TIME_TARGET_MIN * 60;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,reset = %d", __func__,reset);
    if (reset)
    {
        memset(tempSportTargetMoreData, 0, sizeof(ute_module_sport_target_set_data_t) * UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM);
        for (i = 0; i < UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM; i ++)
        {
            tempSportTargetMoreData[i].sportDistanceTargeCnt = uteModuleSportTargetMoreData[i].sportDistanceTargeCnt;
            tempSportTargetMoreData[i].sportTimeTargetSec = uteModuleSportTargetMoreData[i].sportTimeTargetSec;
            tempSportTargetMoreData[i].sportKcalTarget = uteModuleSportTargetMoreData[i].sportKcalTarget;
            tempSportTargetMoreData[i].isNoTarget = uteModuleSportTargetMoreData[i].isNoTarget;
            tempSportTargetMoreData[i].showType = uteModuleSportTargetMoreData[i].showType;
            uteModuleSportTargetMoreData[i].sportDistanceTargeCnt = UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET;
            uteModuleSportTargetMoreData[i].sportTimeTargetSec = timeTargetSec;
            uteModuleSportTargetMoreData[i].sportKcalTarget = UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET;
            uteModuleSportTargetMoreData[i].isNoTarget = 0;
            uteModuleSportTargetMoreData[i].showType = 0;
        }
        for (i = 0; i < UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM; i ++)
        {
            index = uteModuleSportFindHundredSportIndexByMode(tempSportTargetMoreData[i].showType);
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "index = %d", index);
            if (index < UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM)
            {
                uteModuleSportTargetMoreData[index].sportDistanceTargeCnt = tempSportTargetMoreData[i].sportDistanceTargeCnt;;
                uteModuleSportTargetMoreData[index].sportTimeTargetSec = tempSportTargetMoreData[i].sportTimeTargetSec;;
                uteModuleSportTargetMoreData[index].sportKcalTarget = tempSportTargetMoreData[i].sportKcalTarget;;
                uteModuleSportTargetMoreData[index].isNoTarget = tempSportTargetMoreData[i].isNoTarget;
                uteModuleSportTargetMoreData[index].showType = tempSportTargetMoreData[i].showType;
            }
        }
    }
    for (i = 0; i < UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM; i ++)
    {
        /*if (reset)
        {
            writebuff[i * 12 + 0] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET >> 24) & 0xff;
            writebuff[i * 12 + 1] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET >> 16) & 0xff;
            writebuff[i * 12 + 2] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET >> 8) & 0xff;
            writebuff[i * 12 + 3] = UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET & 0xff;
            writebuff[i * 12 + 4] = (timeTargetSec >> 24) & 0xff;
            writebuff[i * 12 + 5] = (timeTargetSec >> 16) & 0xff;
            writebuff[i * 12 + 6] = (timeTargetSec >> 8) & 0xff;
            writebuff[i * 12 + 7] = timeTargetSec & 0xff;
            writebuff[i * 12 + 8] = (UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET >> 8) & 0xff;
            writebuff[i * 12 + 9] = UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET & 0xff;
            writebuff[i * 12 + 10] = 0;
            writebuff[i * 12 + 11] = 0;
            uteModuleSportTargetMoreData[i].sportDistanceTargeCnt = (writebuff[i * 12 + 0] << 24) | (writebuff[i * 12 + 1] << 16) | (writebuff[i * 12 + 2] << 8) | writebuff[i * 12 + 3];
            uteModuleSportTargetMoreData[i].sportTimeTargetSec = (writebuff[i * 12 + 4] << 24) | (writebuff[i * 12 + 5] << 16) | (writebuff[i * 12 + 6] << 8) | writebuff[i * 12 + 7];
            uteModuleSportTargetMoreData[i].sportKcalTarget = (writebuff[i * 12 + 8] << 8) | writebuff[i * 12 + 9];
            uteModuleSportTargetMoreData[i].isNoTarget = writebuff[i * 12 + 10];
            uteModuleSportTargetMoreData[i].showType = writebuff[i * 12 + 11];
        }
        else*/
        {
            writebuff[i * 12 + 0] = (uteModuleSportTargetMoreData[i].sportDistanceTargeCnt >> 24) & 0xff;
            writebuff[i * 12 + 1] = (uteModuleSportTargetMoreData[i].sportDistanceTargeCnt >> 16) & 0xff;
            writebuff[i * 12 + 2] = (uteModuleSportTargetMoreData[i].sportDistanceTargeCnt >> 8) & 0xff;
            writebuff[i * 12 + 3] = (uteModuleSportTargetMoreData[i].sportDistanceTargeCnt) & 0xff;
            writebuff[i * 12 + 4] = (uteModuleSportTargetMoreData[i].sportTimeTargetSec >> 24) & 0xff;
            writebuff[i * 12 + 5] = (uteModuleSportTargetMoreData[i].sportTimeTargetSec >> 16) & 0xff;
            writebuff[i * 12 + 6] = (uteModuleSportTargetMoreData[i].sportTimeTargetSec >> 8) & 0xff;
            writebuff[i * 12 + 7] = (uteModuleSportTargetMoreData[i].sportTimeTargetSec) & 0xff;
            writebuff[i * 12 + 8] = (uteModuleSportTargetMoreData[i].sportKcalTarget >> 8) & 0xff;
            writebuff[i * 12 + 9] = (uteModuleSportTargetMoreData[i].sportKcalTarget) & 0xff;
            writebuff[i * 12 + 10] = uteModuleSportTargetMoreData[i].isNoTarget;
            writebuff[i * 12 + 11] = uteModuleSportTargetMoreData[i].showType;
        }
    }
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_SET_DATA, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writebuff[0], UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM * 12);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief     设置指定数据
*@details
*@author        wuhuowang
*@date        2023-05-08
*/
void uteModuleSportTargetMoreSetMoreFromIndex(uint8_t index)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,index = %d", __func__,index);
    uteModuleSportTargetMoreData[index].sportDistanceTargeCnt = uteModuleSportTargetData.sportDistanceTargeCnt;
    uteModuleSportTargetMoreData[index].sportTimeTargetSec = uteModuleSportTargetData.sportTimeTargetSec;
    uteModuleSportTargetMoreData[index].sportKcalTarget = uteModuleSportTargetData.sportKcalTarget;
    uteModuleSportTargetMoreData[index].isNoTarget = uteModuleSportTargetData.isNoTarget;
    uteModuleSportTargetMoreData[index].showType = uteModuleSportMoreSportGetType();
    uteModuleSportTargetMoreSaveData(false);
}
/**
*@brief     获取指定数据
*@details
*@author        wuhuowang
*@date        2023-05-08
*/
void uteModuleSportTargetMoreGetMoreFromIndex(uint8_t index)
{
    if (index < UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,index = %d dis=%d sec=%d kcal=%d", __func__,index, uteModuleSportTargetMoreData[index].sportDistanceTargeCnt, uteModuleSportTargetMoreData[index].sportTimeTargetSec, uteModuleSportTargetMoreData[index].sportKcalTarget);
        uteModuleSportTargetData.sportDistanceTargeCnt = uteModuleSportTargetMoreData[index].sportDistanceTargeCnt;
        uteModuleSportTargetData.sportTimeTargetSec = uteModuleSportTargetMoreData[index].sportTimeTargetSec;
        uteModuleSportTargetData.sportKcalTarget = uteModuleSportTargetMoreData[index].sportKcalTarget;
        uteModuleSportTargetData.isNoTarget = uteModuleSportTargetMoreData[index].isNoTarget;
    }
    else
    {
        uteModuleSportTargetData.sportDistanceTargeCnt = uteModuleSportTargetMoreData[0].sportDistanceTargeCnt;
        uteModuleSportTargetData.sportTimeTargetSec = uteModuleSportTargetMoreData[0].sportTimeTargetSec;
        uteModuleSportTargetData.sportKcalTarget = uteModuleSportTargetMoreData[0].sportKcalTarget;
        uteModuleSportTargetData.isNoTarget = uteModuleSportTargetMoreData[0].isNoTarget;
    }
}
/**
*@brief        设置目标设置索引
*@details
*@author      wuhuowang
*@date       2023-05-08
*/
void uteModuleSportTargetSetShowIndex(uint8_t index)
{
    if (index <= UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM)
        uteModuleSportTargetData.showIndex = index;
    else
        uteModuleSportTargetData.showIndex = 0;
}
/**
*@brief        获取目标设置索引
*@details
*@author      wuhuowang
*@date       2023-05-08
*/
uint8_t uteModuleSportTargetGetShowIndex(void)
{
    return uteModuleSportTargetData.showIndex;
}
#endif
/**
*@brief        设置运动距离目标的数据
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetDistanceGoal(uint32_t sportDistanceTargeCnt)
{
    uteModuleSportTargetData.sportDistanceTargeCnt = sportDistanceTargeCnt;
    uteModuleSportTargetData.isNoTarget = SPORT_TARGET_DISTANCE_SET;
    uteModuleSportTargetSaveData();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,portDistanceTargetMi = %d", __func__,uteModuleSportTargetData.sportDistanceTargeCnt);
}
/**
*@brief        获取运动距离目标的数据
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint32_t uteModuleSportTargetGetDistanceGoal(void)
{
    uint32_t targetDistance = uteModuleSportTargetData.sportDistanceTargeCnt;
    //if(uteModuleSystemtimeGetDistanceMiType())
    //{
    //    targetDistance = (uint32_t)targetDistance * 0.62177f;
    //}
    return targetDistance;
}
/**
*@brief        设置运动距离目标提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetDistanceGoalNotify(bool isSportDistanceTargetNotify)
{
    uteModuleSportTargetData.isSportDistanceTargetNotify = isSportDistanceTargetNotify;
}
/**
*@brief        获取运动距离目标提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetDistanceGoalNotify(void)
{
    return uteModuleSportTargetData.isSportDistanceTargetNotify;
}
/**
*@brief        设置运动距离目标已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetDistanceGoalHasNotify(bool isSportDistanceTargetHasNotify)
{
    uteModuleSportTargetData.isSportDistanceTargetHasNotify = isSportDistanceTargetHasNotify;
}
/**
*@brief        获取运动距离目标已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetDistanceGoalHasNotify(void)
{
    return uteModuleSportTargetData.isSportDistanceTargetHasNotify;
}
/**
*@brief        设置运动时长目标的数据
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetTimeGoal(uint32_t sportTimeTargetSec)
{
    uteModuleSportTargetData.sportTimeTargetSec = sportTimeTargetSec;
    uteModuleSportTargetData.isNoTarget = SPORT_TARGET_TIME_SET;
    uteModuleSportTargetSaveData();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,sportTimeTargetSec = %d", __func__,uteModuleSportTargetData.sportTimeTargetSec);
}
/**
*@brief        获取运动时长目标的数据
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint32_t uteModuleSportTargetGetTimeGoal(void)
{
    return uteModuleSportTargetData.sportTimeTargetSec;
}
/**
*@brief        设置运动时长目标提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetTimeGoalNotify(bool isSportTimeTargetNotify)
{
    uteModuleSportTargetData.isSportTimeTargetNotify = isSportTimeTargetNotify;
}
/**
*@brief        获取运动时长目标提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetTimeGoalNotify(void)
{
    return uteModuleSportTargetData.isSportTimeTargetNotify;
}

/**
*@brief        设置运动时长目标已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetTimeGoalHasNotify(bool isSportTimeTargetHasNotify)
{
    uteModuleSportTargetData.isSportTimeTargetHasNotify = isSportTimeTargetHasNotify;
}
/**
*@brief        获取运动时长目标已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetTimeGoalHasNotify(void)
{
    return uteModuleSportTargetData.isSportTimeTargetHasNotify;
}

/**
*@brief        设置运动Kcal目标的数据
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetKcalGoal(uint16_t sportKcalTarget)
{
    uteModuleSportTargetData.sportKcalTarget = sportKcalTarget;
    uteModuleSportTargetData.isNoTarget = SPORT_TARGET_KCAL_SET;
    uteModuleSportTargetSaveData();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,sportKcalTarget = %d", __func__,uteModuleSportTargetData.sportKcalTarget);
}
/**
*@brief        获取运动Kcal目标的数据
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint16_t uteModuleSportTargetGetKcalGoal(void)
{
    return uteModuleSportTargetData.sportKcalTarget;
}
/**
*@brief        设置运动Kcal目标提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetKcalGoalNotify(bool isSportKcalTargetNotify)
{
    uteModuleSportTargetData.isSportKcalTargetNotify = isSportKcalTargetNotify;
}
/**
*@brief        获取运动Kcal目标提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetKcalGoalNotify(void)
{
    return uteModuleSportTargetData.isSportKcalTargetNotify;
}
/**
*@brief        设置运动Kcal目标已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetKcalGoalHasNotify(bool isSportKcalTargetHasNotify)
{
    uteModuleSportTargetData.isSportKcalTargetHasNotify = isSportKcalTargetHasNotify;
}
/**
*@brief        获取运动Kcal目标已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetKcalGoalHasNotify(void)
{
    return uteModuleSportTargetData.isSportKcalTargetHasNotify;
}

/**
*@brief        设置运动整一小时提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetOneHourNotify(bool isSportOneHourNotify)
{
    uteModuleSportTargetData.isSportOneHourNotify = isSportOneHourNotify;
}
/**
*@brief        获取运动整一小时提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetOneHourNotify(void)
{
    return uteModuleSportTargetData.isSportOneHourNotify;
}
/**
*@brief        设置运动整一小时已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetOneHourHasNotify(bool isSportOneHourHasNotify)
{
    uteModuleSportTargetData.isSportOneHourHasNotify = isSportOneHourHasNotify;
}
/**
*@brief        获取运动整一小时已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetOneHourHasNotify(void)
{
    return uteModuleSportTargetData.isSportOneHourHasNotify;
}
/**
*@brief        设置运动整一公里提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetOneKmNotify(bool isSportOneKmNotify)
{
    uteModuleSportTargetData.isSportOneKmNotify = isSportOneKmNotify;
}
/**
*@brief        获取运动整一公里提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetOneKmNotify(void)
{
    return uteModuleSportTargetData.isSportOneKmNotify;
}
/**
*@brief        设置运动整一公里已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetOneKmHasNotify(bool isSportOneKmHasNotify)
{
    uteModuleSportTargetData.isSportOneKmHasNotify = isSportOneKmHasNotify;
}
/**
*@brief        获取运动整一公里已经提醒
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleSportTargetGetOneKmHasNotify(void)
{
    return uteModuleSportTargetData.isSportOneKmHasNotify;
}
/**
*@brief        设置无目标
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportTargetSetIsNoTarget(uint8_t isNoTarget)
{
    uteModuleSportTargetData.isNoTarget = isNoTarget;
    uteModuleSportTargetSaveData();
}
/**
*@brief        获取无目标判断
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint8_t uteModuleSportTargetGetIsNoTarget(void)
{
    return uteModuleSportTargetData.isNoTarget;
}
/**
*@brief        设置目标编辑状态
*@details
*@author      wuhuowang
*@date       2023-03-22
*/
void uteModuleSportTargetSetEditStatus(uint8_t status)
{
    uteModuleSportTargetData.editStatus = status;
}
/**
*@brief        获取目标编辑状态
*@details
*@author      wuhuowang
*@date       2023-03-22
*/
uint8_t uteModuleSportTargetGetEditStatus(void)
{
    return uteModuleSportTargetData.editStatus;
}
/**
*@brief        设置目标设置完成后显示类型
*@details
*@author      wuhuowang
*@date       2023-03-22
*/
void uteModuleSportTargetSetShowType(uint8_t type)
{
    uteModuleSportTargetData.showType = type;
}
/**
*@brief        获取目标设置完成后显示类型
*@details
*@author      wuhuowang
*@date       2023-03-22
*/
uint8_t uteModuleSportTargetGetShowType(void)
{
    return uteModuleSportTargetData.showType;
}

