/**
*@file
*@brief     在线表盘模块
*@details   在线表盘模块头文件
*@author        casn
*@date        2021-11-26
*@version       v1.0
*/
#ifndef _UTE_MODULE_WATCHONLINE_H_
#define _UTE_MODULE_WATCHONLINE_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"
#include "include.h"
#if UTE_MODULE_WATCHONLINE_SUPPORT
/*!表盘配置参数 casen, 2021-11-26  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_WATCHONLINE_CONFIG_INFO "systemparam/watchOnlineConfigInfo"

enum
{
    WATCH_ERR_OK = 0,
    WATCH_ERR_CRC = 1,
    WATCH_ERR_FILE_TOO_LARGE = 2,
    WATCH_ERR_NOT_CONTINUOUS = 3,
    WATCH_ERR_CONTINUOUS = 4,
};

enum
{
    WATCH_LEVEL0 = 0,
    WATCH_LEVEL1 = 1,  //支持小指针
    WATCH_LEVEL2 = 2,  //支持压缩
    WATCH_LEVEL3 = 3, //支持卡路里百分比，距离百分比
    WATCH_LEVELMAX,
};

typedef struct
{
    uint32_t snNo;
    uint32_t fileSize;
    uint32_t fileCrc;// no crc this config
    uint16_t pixelWidth;
    uint16_t pixelHeight;
    uint8_t screenType;
    uint8_t hasBg;
    uint8_t isWatchVaild;
    uint8_t supportLevel;
    uint8_t reserved[4];
} watchConfig_t; // all 24byte write to file

#if UTE_MODULE_WATCH_PHOTO_SUPPORT
typedef struct
{
    uint8_t isVaild;
    uint8_t timePosition; // 时间位置
    uint32_t fontColor;   // 字体颜色
    uint32_t bgSize;
    uint32_t previewSize;
} photoWatchConfig_t;
#endif

typedef struct
{
    watchConfig_t watchConfig;
    uint8_t validPicConfigCnt;
    uint32_t fileCrc32;
    uint32_t fileSize;
    uint32_t lastEraseAddress;
    bool isStartReceive;
    bool isStartCrc32;
    uint32_t receiveCnt;
    int lastReceiveIndex;
    uint8_t receiveTimeout;
    uint8_t supportMultipleMaxCnt;//最大支持数量
    int8_t currDisplayIndex;//当前显示在线表盘索引
    uint8_t writeWatchIndex;//当前写入在线表盘索引
    uint8_t multipleValidWatchCnt;//有效在线表盘数量
    uint8_t multipleWatchIndexBuff[UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT];
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
    bool isStartReceivePhoto;
    bool isHasPhoto;
    uint8_t photoWatchIndex;
    uint8_t photoWatchTimePosition; // 时间位置
    uint32_t photoWatchFontColor;   // 字体颜色
    uint8_t photoWatchPictureCnt;
    uint8_t photoWatchPictureIndex;
    photoWatchConfig_t photoWatchConfig[UTE_MODULE_WATCH_PHOTO_MAX_PICTURE_CNT];
    watchConfig_t ReceivePhotoHead;
#endif
#if UTE_MODULE_WATCHONLINE_WRITE_INDEX_AUTO_SUPPORT
    uint8_t lastWriteWatchIndex;
#endif
} ute_module_watchonline_data_t;
void uteModuleWatchOnlineDataInit(void);
uint32_t uteModuleWatchOnlineGetBaseAddress(uint8_t index);
void uteModuleWatchOnlineReadDeviceInfo(uint8_t *data);
void uteModuleWatchOnlineUpateConfigFromFlash(void);
void uteModuleWatchOnlineReadyStart(void);
uint8_t uteModuleWatchOnlineDataWrite(const uint8_t *data,uint32_t size);
void uteModuleWatchOnlineSyncData(uint8_t*receive,uint8_t length);
uint8_t uteModuleWatchOnLineTSyncComplete(void);
void uteModuleWatchOnlineRecvTimeoutMsgHandler(void);
void uteModuleWatchOnlineOneSecMsgHandler(void);
bool uteModuleWatchOnlineUpddateDefaultWatchIndex(void);
/**
*@brief      在线表盘总大小
*@details   获取当前同步的在线表盘总的大小
*@return    online watch totile size
*@author     ellison
*@date       2022-06-07
*/
uint32_t uteModuleWatchOnlineGetTotileWatchSize(void);

/**
*@brief      在线表盘同步时的大小
*@details   获取当前同步的在线表盘已经同步了多少
*@return     online watch Synchronize size
*@author     ellison
*@date       2022-06-07
*/
uint32_t uteModuleWatchOnlineGetSynchronizeWatchSize(void);
uint8_t uteModuleWatchOnlineGetSupportMultipleCnt(void);
uint8_t uteModuleWatchOnlineGetVailWatchCnt(void);
#if ((UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT!=1))
uint8_t uteModuleWatchOnlineGetWatchindex(uint8_t index);
void uteModuleWatchOnlineSwitchWatchMain(uint8_t watchIndex, uint32_t watchId);
#endif
void uteModuleWatchOnlineGetInfoWithIndex(uint8_t index, uint8_t *data);
void uteModuleWatchOnlineSetWillUpdateDataIndex(uint8_t index);
void uteModuleWatchOnlineStartSync(void);
void uteModuleWatchOnlineDeleteDataIndex(uint8_t index, uint8_t *data);
void uteModuleWatchOnlineDeleteDataMultipleIndex(uint8_t count, uint8_t *index);
void uteModuleWatchOnlineGetAllInfoStart(void);
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
uint32_t uteModuleWatchOnlineGetOnePhotoMaxSize(void);
void uteModuleWatchOnlineUpdatePhotoWatchInfo(void);
void uteModuleWatchOnlineGetInfoWithPhoto(uint8_t *data);
void uteModuleWatchOnlineStartSyncPhoto(uint8_t *data);
void uteModuleWatchOnlineSetStartReceivePhoto(bool isStart);
void uteModuleWatchOnlineGetPhotoAddress(uint8_t index, uint32_t *bassAddr, uint32_t *preview, uint32_t *photo);
void uteModuleWatchOnlineGetCurrPhotoAddress(uint32_t *preview, uint32_t *photo);
bool uteModuleWatchOnlineIsHasPhoto(void);
void uteModuleWatchOnlineGetCurrPhotoWatchConfig(photoWatchConfig_t *config);
#endif
void uteModuleWatchOnlineSetInfo(uint8_t *data);
bool uteModuleWatchOnlinegetIsStartReceive(void);
#if UTE_MODULE_WATCHONLINE_WRITE_INDEX_AUTO_SUPPORT
void uteModuleWatchOnlineWriteIndexAutoConfirm(void);
#endif
#endif
#endif //_UTE_MODULE_WATCHONLINE_H_

