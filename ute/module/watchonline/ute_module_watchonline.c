/**
 *@file
 *@brief     在线表盘模块
 *@details
 *@author        casen
 *@date        2021-11-26
 *@version       v1.0
 */
#include "ute_module_heart.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_watchonline.h"
#include "ute_module_protocol.h"
#include "ute_module_crc.h"
#include "ute_module_gui_common.h"
#include "ute_module_platform.h"
#include "ute_module_call.h"
#include "ute_module_sport.h"
#include "ute_module_message.h"
#include "include.h"

#if UTE_MODULE_WATCHONLINE_SUPPORT

const uint32_t uteModuleWatchOnlineMultipleBaseAddress[UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT] = UTE_MODULE_WATCHONLINE_MULTIPLE_BASE_ADDRESS_ARRAYS;

extern void func_clock_recreate_dial(void);

/*! 在线表盘的数据 casen, 2021-11-27  */
ute_module_watchonline_data_t uteModuleWatchOnlineData;

/*! 接收单笔数据超时指针, 2021-11-27  */
void *uteModuleWatchOnlineRecvTimeoutTimerPointer = NULL;
void *uteModuleWatchOnlineOneSecTimerPointer = NULL;
void *uteModuleWatchOnlineWaitingStartTimerPointer = NULL;
void *uteModuleWatchOnlineWaitingDellTimerPointer = NULL;

void uteModuleWatchOnlineDataInit(void)
{
    memset(&uteModuleWatchOnlineData, 0, sizeof(ute_module_watchonline_data_t));
}

/**
 * @brief        获取在线表盘地址
 * @details
 * @return       在线表盘地址
 * @author       Wang.Luo
 * @date         2024-11-26
 */
uint32_t uteModuleWatchOnlineGetBaseAddress(uint8_t index)
{
    if (index > sizeof(uteModuleWatchOnlineMultipleBaseAddress) / sizeof(uint32_t) - 1)
    {
        return 0;
    }
    else
    {
        return uteModuleWatchOnlineMultipleBaseAddress[index];
    }
}

/**
 *@brief      在线表盘总大小
 *@details    获取当前同步的在线表盘总的大小
 *@return     online watch totile size
 *@author     ellison
 *@date       2022-06-07
 */
uint32_t uteModuleWatchOnlineGetTotileWatchSize(void)
{
    int watchConfigSize = sizeof(watchConfig_t);
    watchConfig_t watchConfig;
    uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex], watchConfigSize);
    return watchConfig.fileSize;
}

/**
 *@brief      在线表盘同步时的大小
 *@details    获取当前同步的在线表盘已经同步了多少
 *@return     online watch updateing size
 *@author     ellison
 *@date       2022-06-07
 */
uint32_t uteModuleWatchOnlineGetSynchronizeWatchSize(void)
{
    return uteModuleWatchOnlineData.fileSize;
}

/**
 *@brief      在线表盘定时器回调
 *@details
 *@author     casen
 *@date       2021-11-27
 */
void uteModuleWatchOnlineRecvTimeoutTimerCallback(void *pxTimer)
{
    ute_ble_connect_state_t state;
    uteApplicationCommonGetBleConnectionState(&state);
    if (state.isConnected == true)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_WATCHONLINE_RECVTIMEOUT_TIMER, 0);
    }
    else
    {
        uteModulePlatformStopTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
    }
}

void uteModuleWatchOnlineOneSecCallback(void *pxTimer)
{
    ute_ble_connect_state_t state;
    uteApplicationCommonGetBleConnectionState(&state);
    if (state.isConnected == true)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_WATCHONLINE_ONESEC_TIMER, 0);
    }
    else
    {
        uteModulePlatformStopTimer(&uteModuleWatchOnlineOneSecTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineOneSecTimerPointer);
    }
}

void uteModuleWatchOnlineWaitingStartCallback(void *pxTimer)
{
    uint8_t watchIndex = 0;
    uteModuleGuiCommonGetCurrWatchIndex(&watchIndex);
    // 等待当前表盘不是要同步的表盘后再擦除flash
    if (uteModuleWatchOnlineData.isStartReceive && ((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_WATCHMAIN_ID &&
            (watchIndex - UTE_MODULE_SCREENS_WATCH_CNT_MAX) == uteModuleWatchOnlineData.writeWatchIndex) ||
            uteModuleGuiCommonGetCurrentScreenId() == FUNC_CLOCK_PREVIEW))
    {
        uteModulePlatformRestartTimer(&uteModuleWatchOnlineWaitingStartTimerPointer, 100);
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,waiting to switch dials...", __func__);
    }
    else
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_WATCHONLINE_START_TIMER, 0);
        uteModulePlatformStopTimer(&uteModuleWatchOnlineWaitingStartTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineWaitingStartTimerPointer);
        uteModuleWatchOnlineWaitingStartTimerPointer = NULL;
    }
}

void uteModuleWatchOnlineStartSync(void)
{
    uint8_t response[3];
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
    if (uteModuleWatchOnlineData.isStartReceivePhoto)
    {
        uint32_t bassAddr = 0;
        uint32_t preview = 0;
        uint32_t photo = 0;
        uteModuleWatchOnlineGetPhotoAddress(uteModuleWatchOnlineData.photoWatchPictureIndex, &bassAddr, &preview, &photo);

        uteModulePlatformFlashNorErase(uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.photoWatchIndex]);
        uteModulePlatformFlashNorWrite((uint8_t *)&uteModuleWatchOnlineData.ReceivePhotoHead, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.photoWatchIndex], sizeof(watchConfig_t));

        uteModuleWatchOnlineData.lastEraseAddress = bassAddr;
        uteModulePlatformFlashNorErase(uteModuleWatchOnlineData.lastEraseAddress);
        response[0] = CMD_WATCH_ONLINE;
        response[1] = 0x0b;
        response[2] = 0x00;
        uteModuleProfileBleSendToPhone(&response[0], 3);
    }
    else
#endif
    {
        uteModulePlatformFlashNorErase(uteModuleWatchOnlineData.lastEraseAddress);
        response[0] = CMD_WATCH_ONLINE;
        response[1] = 0x02;
        uteModuleProfileBleSendToPhone(&response[0], 2);
    }
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,start sync...", __func__);
}

void uteModuleWatchOnlineRecvTimeoutMsgHandler(void)
{
    uint8_t response[20];
    uteModuleWatchOnlineData.receiveTimeout = 1; // Casen add 21-04-23
    response[0] = CMD_WATCH_ONLINE;
    response[1] = 0x03;
    response[2] = WATCH_ERR_NOT_CONTINUOUS;
    response[3] = (uteModuleWatchOnlineData.lastReceiveIndex + 1) >> 8 & 0xff;
    response[4] = (uteModuleWatchOnlineData.lastReceiveIndex + 1) & 0xff;
    if (uteModuleWatchOnlineData.isStartReceive == true)
    {
        uteModuleProfileBleSendToPhone(&response[0], 5);
    }
}

void uteModuleWatchOnlineOneSecMsgHandler(void)
{
    if (uteModuleWatchOnlineData.receiveTimeout != 0)
    {
        uint8_t response[3] = {};
        uteModuleWatchOnlineData.receiveTimeout += 1;
        if (uteModuleWatchOnlineData.receiveTimeout == 4)
        {
            uteModulePlatformStopTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
            response[0] = CMD_WATCH_ONLINE;
            response[1] = 0x03;
            response[2] = WATCH_ERR_CONTINUOUS;
            uteModuleProfileBleSendToPhone(&response[0], 3);
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,WATCH_ERR_CONTINUOUS", __func__);
        }
        else if (uteModuleWatchOnlineData.receiveTimeout >= 6)
        {
            uteModuleWatchOnlineData.isStartReceive = false;
            response[0] = CMD_WATCH_ONLINE;

            response[1] = 0x03;
            response[2] = WATCH_ERR_CRC;
            uteModuleProfileBleSendToPhone(&response[0], 3);
            uteModuleWatchOnlineUpddateDefaultWatchIndex();
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
            /**同步表盘超时时候，出现超时，则退出表盘界面，防止APP同步表盘时退出未被杀死，导致卡在表盘同步界面**/
            //            if((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_CALL_INCOMING_ID ))
            //            {
            //                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CALL_INCOMING_ID);
            //            }
            //            else if((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_CALL_OUTGOING_ID ))
            //            {
            //                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CALL_OUTGOING_ID);
            //            }
            //            else if((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_CALL_ING_ID ))
            //            {
            //                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CALL_ING_ID);
            //            }
            //            else
            //            {
            uteModuleGuiCommonGoBackLastScreen();
//            }
#endif
            uteModuleWatchOnlineData.receiveTimeout = 0;
        }
    }
    else
    {
        uteModulePlatformStopTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
        uteModulePlatformStopTimer(&uteModuleWatchOnlineOneSecTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineOneSecTimerPointer);
        uteModuleWatchOnlineRecvTimeoutTimerPointer = NULL;
        uteModuleWatchOnlineOneSecTimerPointer = NULL;
    }
}
/**
 *@brief        多个在线表盘配置
 *@details      更新表盘信息
 *@author       zn.zeng
 *@date       2022-06-28
 */
void uteModuleWatchOnlineUpateConfigFromFlash(void)
{
    uint8_t addressCnt = sizeof(uteModuleWatchOnlineMultipleBaseAddress) / sizeof(uint32_t);
    if (addressCnt > UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uteModuleWatchOnlineData.supportMultipleMaxCnt = UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT;
    }
    else
    {
        uteModuleWatchOnlineData.supportMultipleMaxCnt = addressCnt;
    }
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
    uteModuleWatchOnlineUpdatePhotoWatchInfo();
#endif
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,supportMultipleMaxCnt=%d\r\n", __func__, uteModuleWatchOnlineData.supportMultipleMaxCnt);
    int watchConfigSize = sizeof(watchConfig_t);
    // int picConfigSize = sizeof(watchConfig_t);
    uteModuleWatchOnlineData.validPicConfigCnt = 0;
    watchConfig_t config;
    uint8_t vailWatch = 0;
    bool isConnectOurApp = uteApplicationCommonIsHasConnectOurApp();
    for (uint8_t i = 0; i < uteModuleWatchOnlineData.supportMultipleMaxCnt; i++)
    {
        uteModulePlatformFlashNorRead((uint8_t *)&config, uteModuleWatchOnlineMultipleBaseAddress[i], watchConfigSize);
        uint16_t headerNum = bsp_uitool_header_phrase(uteModuleWatchOnlineMultipleBaseAddress[i]);
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
        if (config.snNo == UTE_MODULE_WATCH_PHOTO_DEFAULT_ID && uteModuleWatchOnlineData.isHasPhoto)
        {
            headerNum = 1;
        }
#endif
        if (config.isWatchVaild == 0 && headerNum)
        {
            if (isConnectOurApp)
            {
                uteModuleWatchOnlineData.multipleWatchIndexBuff[vailWatch] = i;
                vailWatch++;
            }
            else
            {
                uteModulePlatformFlashNorErase(uteModuleWatchOnlineMultipleBaseAddress[i]);
            }
        }
    }
    uteModuleWatchOnlineData.multipleValidWatchCnt = vailWatch;
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,multipleValidWatchCnt=%d\r\n", __func__, uteModuleWatchOnlineData.multipleValidWatchCnt);
    if (!isConnectOurApp)
    {
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,is not connect our app!", __func__);
    }
    uint8_t watchIndex = 0;
    uteModuleGuiCommonGetCurrWatchIndex(&watchIndex);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,watchIndex=%d\r\n", __func__, watchIndex);
    if (uteModuleWatchOnlineData.multipleValidWatchCnt == 0)
    {
        if (watchIndex >= UTE_MODULE_SCREENS_WATCH_CNT_MAX)
        {
            printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy\r\n");
            uteModuleGuiCommonSetCurrWatchIndex(DEFAULT_WATCH_INDEX);
        }
    }
    else
    {
        if (watchIndex >= UTE_MODULE_SCREENS_WATCH_CNT_MAX)
        {
            uteModuleWatchOnlineData.currDisplayIndex = uteModuleWatchOnlineData.multipleWatchIndexBuff[watchIndex - UTE_MODULE_SCREENS_WATCH_CNT_MAX];
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,currDisplayIndex=%d\r\n", __func__, uteModuleWatchOnlineData.currDisplayIndex);
            uteModulePlatformFlashNorRead((uint8_t *)&uteModuleWatchOnlineData.watchConfig, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.currDisplayIndex], watchConfigSize);
        }
    }
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,validPicConfigCnt=%d\r\n", __func__, uteModuleWatchOnlineData.validPicConfigCnt);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,validPicConfigCnt=%d,hasbg=%d\r\n", __func__, uteModuleWatchOnlineData.validPicConfigCnt, uteModuleWatchOnlineData.watchConfig.hasBg);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,snNo=%d,pixelWidth=%d,pixelHeight=%d,filesize=%d\r\n", __func__, uteModuleWatchOnlineData.watchConfig.snNo, uteModuleWatchOnlineData.watchConfig.pixelWidth, uteModuleWatchOnlineData.watchConfig.pixelHeight, uteModuleWatchOnlineData.watchConfig.fileSize);
}

void uteModuleWatchOnlineReadDeviceInfo(uint8_t *data)
{
    if (uteModuleWatchOnlineData.multipleValidWatchCnt)
    {
        int watchConfigSize = sizeof(watchConfig_t);
        uteModulePlatformFlashNorRead((uint8_t *)&uteModuleWatchOnlineData.watchConfig, uteModuleWatchOnlineMultipleBaseAddress[0], watchConfigSize);
        data[0] = (uteModuleWatchOnlineData.watchConfig.snNo >> 24) & 0xff;
        data[1] = (uteModuleWatchOnlineData.watchConfig.snNo >> 16) & 0xff;
        data[2] = (uteModuleWatchOnlineData.watchConfig.snNo >> 8) & 0xff;
        data[3] = uteModuleWatchOnlineData.watchConfig.snNo & 0xff;
    }
    else
    {
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = 0xff;
        data[3] = 0xff;
    }
    data[4] = (UTE_DRV_SCREEN_WIDTH >> 8) & 0xff;
    data[5] = UTE_DRV_SCREEN_WIDTH & 0xff;
#if UTE_DRV_SCREEN_WATCH_ONLINE_HEIGHT_HAND
    data[6] = ((UTE_DRV_SCREEN_HEIGHT - 1) >> 8) & 0xff;
    data[7] = (UTE_DRV_SCREEN_HEIGHT - 1) & 0xff;
#else
    data[6] = (UTE_DRV_SCREEN_HEIGHT >> 8) & 0xff;
    data[7] = UTE_DRV_SCREEN_HEIGHT & 0xff;
#endif

    data[8] = UTE_DRV_SCREEN_SHAPE;
    data[9] = ((UTE_MODULE_WATCHONLINE_MAX_SIZE / UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT) >> 24) & 0xff;
    data[10] = ((UTE_MODULE_WATCHONLINE_MAX_SIZE / UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT) >> 16) & 0xff;
    data[11] = ((UTE_MODULE_WATCHONLINE_MAX_SIZE / UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT) >> 8) & 0xff;
    data[12] = (UTE_MODULE_WATCHONLINE_MAX_SIZE / UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT) & 0xff;
    data[13] = UTE_MODULE_WATCHONLINNE_COMPATIBLE_LEVEL;
    data[14] = UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT;
#if UTE_MODULE_WATCHONLINNE_R_VALUE
    data[15] = UTE_MODULE_WATCHONLINNE_R_VALUE;
#endif
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
    data[16] = UTE_MODULE_WATCH_PHOTO_SUPPORT;
#endif
}

void uteModuleWatchOnlineReadyStart(void)
{
    if (uteModuleWatchOnlineRecvTimeoutTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer, "WatchOnlineRecvTimeoutTimer", 1, 300, true, uteModuleWatchOnlineRecvTimeoutTimerCallback);
    }
    if (uteModuleWatchOnlineOneSecTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleWatchOnlineOneSecTimerPointer, "WatchOnlineOneSecTimer", 2, 1000, true, uteModuleWatchOnlineOneSecCallback);
    }
    if(uteModuleWatchOnlineWaitingStartTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleWatchOnlineWaitingStartTimerPointer, "WatchOnlineWaitingStartTimer", 1, 100, false, uteModuleWatchOnlineWaitingStartCallback);
    }
    if(uteModuleWatchOnlineOneSecTimerPointer != NULL)
    {
        uteModulePlatformRestartTimer(&uteModuleWatchOnlineOneSecTimerPointer, 1000);
    }
    if(uteModuleWatchOnlineWaitingStartTimerPointer != NULL)
    {
        uteModulePlatformRestartTimer(&uteModuleWatchOnlineWaitingStartTimerPointer,100);
    }
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL,"%s,.writeWatchIndex=%d,supportMultipleMaxCnt=%d",__func__,uteModuleWatchOnlineData.writeWatchIndex,uteModuleWatchOnlineData.supportMultipleMaxCnt);
    if (uteModuleWatchOnlineData.writeWatchIndex >= UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uteModuleWatchOnlineData.writeWatchIndex = 0;
    }
    uteModuleWatchOnlineData.receiveCnt = 0;
    uteModuleWatchOnlineData.fileCrc32 = 0xffffffff;
    uteModuleWatchOnlineData.isStartReceive = true;
    uteModuleWatchOnlineData.fileSize = 0;
    uteModuleWatchOnlineData.isStartCrc32 = false;
    uteModuleWatchOnlineData.lastReceiveIndex = -1;
    uteModuleWatchOnlineData.lastEraseAddress = uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex];
    if(uteModuleWatchOnlineData.multipleValidWatchCnt)uteModuleWatchOnlineData.multipleValidWatchCnt--;// cxd add 2022-08-24
    uteModuleWatchOnlineData.receiveTimeout = 1; //Casen add 2020-10-26
    bool isUpdateWatch = uteModuleWatchOnlineUpddateDefaultWatchIndex();

    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,need erase lastEraseAddress=0x%x\r\n", __func__, uteModuleWatchOnlineData.lastEraseAddress);

    // 先跳转到主表盘界面
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
    // if(!uteModuleGuiCommonIsDisplayOn())


    {
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SYNC_WATCH_ONLINE_ID, 0, __func__);
    }
#else
    if (isUpdateWatch || !uteModuleGuiCommonIsDisplayOn())
    {
        if(uteModuleGuiCommonGetCurrentScreenId() != UTE_MOUDLE_SCREENS_WATCHMAIN_ID)
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
        }
        else
        {
            func_clock_recreate_dial();
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = true;
            }
            reset_sleep_delay_all();
        }
    }
#endif
    //Casen add 2020-02-24
    // uteModulePlatformFlashNorErase(uteModuleWatchOnlineData.lastEraseAddress);
}

uint8_t uteModuleWatchOnlineDataWrite(const uint8_t *data, uint32_t size)
{
    uint8_t wConfigSize = sizeof(watchConfig_t);
    uint32_t writeAddress = uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex]+uteModuleWatchOnlineData.fileSize;
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
    if(uteModuleWatchOnlineData.isStartReceivePhoto)
    {
        uint32_t bassAddr = 0;
        uint32_t preview = 0;
        uint32_t photo = 0;
        uteModuleWatchOnlineGetPhotoAddress(uteModuleWatchOnlineData.photoWatchPictureIndex, &bassAddr, &preview, &photo);
        if(uteModuleWatchOnlineData.fileSize)
        {
            writeAddress = bassAddr + sizeof(photoWatchConfig_t) + uteModuleWatchOnlineData.fileSize;
        }
        else
        {
            writeAddress = bassAddr;
        }
    }
#endif
    uint32_t eraseAddress;
    eraseAddress = ((writeAddress + size) / 0x1000) * 0x1000;
    if (eraseAddress >= (uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex] + (UTE_MODULE_WATCHONLINE_MAX_SIZE / UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)))
    {
        uteModuleWatchOnlineData.isStartReceive = false;
        uteModuleWatchOnlineUpddateDefaultWatchIndex();
        return WATCH_ERR_FILE_TOO_LARGE;
    }
    uteModuleWatchOnlineData.receiveCnt++;
    // write to flash
    if (eraseAddress != uteModuleWatchOnlineData.lastEraseAddress)
    {
        uint8_t response[10];
        response[0] = CMD_WATCH_ONLINE;
        response[1] = 0x03;
        response[2] = WATCH_ERR_CONTINUOUS;
        uteModuleWatchOnlineData.lastEraseAddress = eraseAddress;
        uteModulePlatformFlashNorErase(uteModuleWatchOnlineData.lastEraseAddress);
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,need erase lastEraseAddress=0x%x\r\n", __func__, uteModuleWatchOnlineData.lastEraseAddress);
        uteModuleProfileBleSendToPhone(&response[0], 3);
    }
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
    if (uteModuleWatchOnlineData.isStartReceivePhoto && uteModuleWatchOnlineData.fileSize == 0)
    {
        uteModulePlatformFlashNorWrite((uint8_t *)&uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex], writeAddress, sizeof(photoWatchConfig_t));
        writeAddress += sizeof(photoWatchConfig_t);
        uteModulePlatformFlashNorWrite((uint8_t *)(&data[0]), writeAddress, size);
    }
    else
#endif
    {
        uteModulePlatformFlashNorWrite((uint8_t *)(&data[0]), writeAddress, size);
    }
    uteModuleWatchOnlineData.fileSize += size;
    // write to flash
    //  crc
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
    if(uteModuleWatchOnlineData.isStartReceivePhoto)
    {
        uteModuleCrc32Bit(&data[0], size, &uteModuleWatchOnlineData.fileCrc32);
    }
    else
#endif
    {
        if (uteModuleWatchOnlineData.fileSize > wConfigSize)
        {
            if (uteModuleWatchOnlineData.isStartCrc32)
            {
                uteModuleCrc32Bit(&data[0], size, &uteModuleWatchOnlineData.fileCrc32);
            }
            else
            {
                uint8_t needCrcCnt = 0;
                uteModuleWatchOnlineData.isStartCrc32 = true;
                if ((uteApplicationCommonGetMtuSize() - 1) < wConfigSize)
                {
                    needCrcCnt = uteModuleWatchOnlineData.fileSize % wConfigSize;
                }
                else if ((uteApplicationCommonGetMtuSize() - 1) == wConfigSize)
                {
                    needCrcCnt = size;
                }
                else
                {
                    needCrcCnt = uteModuleWatchOnlineData.fileSize - wConfigSize;
                }
                UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,needCrcCnt=%d,size=%d\r\n", __func__, needCrcCnt, size);
                uteModuleCrc32Bit(&data[size - needCrcCnt], needCrcCnt, &uteModuleWatchOnlineData.fileCrc32);
            }
        }
    }
    // crc
    //    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,uteModuleWatchOnlineData.fileSize=%d\r\n", __func__,uteModuleWatchOnlineData.fileSize);
    return WATCH_ERR_OK;
}

void uteModuleWatchOnlineSyncData(uint8_t *receive, uint8_t length)
{
    uint8_t response[10];
    if (uteModuleWatchOnlineData.isStartReceive)
    {
        int index = receive[1] << 8 | receive[2];
        uteModuleWatchOnlineData.receiveTimeout = 1; // Casen add 2020-10-26
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "uteModuleWatchOnlineData.lastReceiveIndex=%d,index=%d", uteModuleWatchOnlineData.lastReceiveIndex, index);
        if ((index - uteModuleWatchOnlineData.lastReceiveIndex) != 1)
        {
            uteModulePlatformRestartTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer, 300);
            return;
        }
        uteModulePlatformStopTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
        uteModuleWatchOnlineData.lastReceiveIndex = index;
        if (uteModuleWatchOnlineDataWrite(&receive[3], length - 3) == WATCH_ERR_FILE_TOO_LARGE)
        {
            response[0] = CMD_WATCH_ONLINE;
            response[1] = 0x03;
            response[2] = WATCH_ERR_FILE_TOO_LARGE;
            uteModuleProfileBleSendToPhone(&response[0], 3);
        }
#if UTE_BT30_CALL_SUPPORT //&& (UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT == false)
        uint disp_status = bsp_bt_disp_status();
        if (uteModuleGuiCommonGetCurrentScreenId() == FUNC_BT_CALL || disp_status >= BT_STA_INCOMING)
        {
            response[0] = CMD_WATCH_ONLINE;
            response[1] = 0x03;
            response[2] = WATCH_ERR_CRC;
            uteModuleProfileBleSendToPhone(&response[0],3);
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL,"%s,sync watchonline error because BT3.0 is calling!",__func__);
        }
#endif
    }
}
extern bool slider_unlock;
uint8_t uteModuleWatchOnLineTSyncComplete(void)
{
    uint8_t status = WATCH_ERR_OK;
    int watchConfigSize = sizeof(watchConfig_t);
    watchConfig_t watchConfig;
    uteModuleWatchOnlineData.fileCrc32 = uteModuleWatchOnlineData.fileCrc32 ^ 0xffffffff;
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,systemNotSaveVariable.writeWatchIndex=0x%x", __func__, uteModuleWatchOnlineData.writeWatchIndex);
    uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex], watchConfigSize);
    uteModuleWatchOnlineData.isStartReceive = false;
    uteModuleWatchOnlineData.receiveTimeout = 0; // Casen add 20-10-28
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,systemNotSaveVariable.watchParam.fileCrc32=0x%x,fileSize=%d", __func__, uteModuleWatchOnlineData.fileCrc32, uteModuleWatchOnlineData.fileSize);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,systemNotSaveVariable.watchConfig.fileCrc32=0x%x,fileSize=%d,snNo=%d", __func__, watchConfig.fileCrc, watchConfig.fileSize,watchConfig.snNo);
    uint16_t headerNum = bsp_uitool_header_phrase(uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex]);
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
    uint32_t bassAddr = 0;
    uint32_t preview = 0;
    uint32_t photo = 0;
    uteModuleWatchOnlineData.isStartReceivePhoto = false;
    uteModuleWatchOnlineGetPhotoAddress(uteModuleWatchOnlineData.photoWatchPictureIndex, &bassAddr, &preview, &photo);
    if (watchConfig.snNo == UTE_MODULE_WATCH_PHOTO_DEFAULT_ID)
    {
        area_t img_area;
        img_area = gui_image_get_size(photo);
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL,"%s,bassAddr=%x,preview=%x,photo=%x,wid=%d,hei=%d",__func__,bassAddr,preview,photo,img_area.wid,img_area.hei);
        if (img_area.wid > 0 && img_area.hei > 0)
        {
            headerNum = 1;
        }
    }
#endif
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,headerNum=%d", __func__, headerNum);
    if (uteModuleWatchOnlineData.fileCrc32 == watchConfig.fileCrc && headerNum > 0)
    {
        watchConfig.isWatchVaild = 0;
        uteModulePlatformFlashNorWrite((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex], watchConfigSize);
#if ((UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT != 1))
        uteModuleGuiCommonSetCurrWatchIndex(uteModuleWatchOnlineGetWatchindex(uteModuleWatchOnlineData.writeWatchIndex) + UTE_MODULE_SCREENS_WATCH_CNT_MAX - 1);
#else
        uteModuleGuiCommonSetCurrWatchIndex(UTE_MODULE_SCREENS_WATCH_CNT_MAX + uteModuleWatchOnlineData.writeWatchIndex);
#endif
    }
    else
    {
        uteModuleWatchOnlineUpddateDefaultWatchIndex();
        status = WATCH_ERR_CRC;
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
        if (watchConfig.snNo == UTE_MODULE_WATCH_PHOTO_DEFAULT_ID)
        {
            uteModulePlatformFlashNorErase(bassAddr);
        }
#endif
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,Sync watchonline error because crc error!", __func__);
    }
    uteModuleWatchOnlineUpateConfigFromFlash();
    // if (slider_unlock == false)
    // {
    //     printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\r\n");
    //     uteModuleGuiCommonGoBackLastScreen();
    // }
    /*判断如果再运动中，则会=返回上个界面，防止运动被中断*/
#if UTE_MODULE_SPORT_SUPPORT
    if (uteModuleSportMoreSportIsRuning())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
    else
#endif
    {
        if(uteModuleGuiCommonGetCurrentScreenId() != UTE_MOUDLE_SCREENS_WATCHMAIN_ID)
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
        }
        else
        {
            func_clock_recreate_dial();
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = true;
            }
            reset_sleep_delay_all();
        }
    }
    return status;
}

/**
 *@brief        获取在线表盘最大支持数
 *@details
 *@author       zn.zeng
 *@date       2022-06-28
 */
uint8_t uteModuleWatchOnlineGetSupportMultipleCnt(void)
{
    return uteModuleWatchOnlineData.supportMultipleMaxCnt;
}

/**
 *@brief        获取在线表盘有效表盘数
 *@details
 *@author       zn.zeng
 *@date       2022-06-28
 */
uint8_t uteModuleWatchOnlineGetVailWatchCnt(void)
{
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,multipleValidWatchCnt=%d", __func__, uteModuleWatchOnlineData.multipleValidWatchCnt);
    return uteModuleWatchOnlineData.multipleValidWatchCnt;
}

#if ((UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT != 1))
/**
 *@brief       获取当前表盘显示编号
 *@details
 *@param[in] uint8_t index,预览索引
 *@author     pcm
 *@date       2023-01-03
 */
uint8_t uteModuleWatchOnlineGetWatchindex(uint8_t index)
{
    uint8_t addressCnt = sizeof(uteModuleWatchOnlineMultipleBaseAddress) / sizeof(uint32_t);
    if (addressCnt > UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uteModuleWatchOnlineData.supportMultipleMaxCnt = UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT;
    }
    else
    {
        uteModuleWatchOnlineData.supportMultipleMaxCnt = addressCnt;
    }
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,supportMultipleMaxCnt=%d\r\n", __func__, uteModuleWatchOnlineData.supportMultipleMaxCnt);
    int watchConfigSize = sizeof(watchConfig_t);
    //int picConfigSize = sizeof(watchConfig_t);
    uteModuleWatchOnlineData.validPicConfigCnt = 0;
    watchConfig_t config;
    uint8_t vailWatch = 0;
    bool isConnectOurApp = uteApplicationCommonIsHasConnectOurApp();
    for (uint8_t i = 0; i < uteModuleWatchOnlineData.supportMultipleMaxCnt; i++)
    {
        uteModulePlatformFlashNorRead((uint8_t *)&config, uteModuleWatchOnlineMultipleBaseAddress[i], watchConfigSize);
        uint16_t headerNum = bsp_uitool_header_phrase(uteModuleWatchOnlineMultipleBaseAddress[i]);
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
        if (config.snNo == UTE_MODULE_WATCH_PHOTO_DEFAULT_ID)
        {
            headerNum = 1;
        }
#endif
        if (config.isWatchVaild == 0 && headerNum)
        {
            if (isConnectOurApp)
            {
                uteModuleWatchOnlineData.multipleWatchIndexBuff[vailWatch] = i;
                UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,multipleWatchIndexBuff=%d,vailWatch=%d\r\n", __func__, uteModuleWatchOnlineData.multipleWatchIndexBuff[vailWatch], vailWatch);
                vailWatch++;
            }
            else
            {
                uteModulePlatformFlashNorErase(uteModuleWatchOnlineMultipleBaseAddress[i]);
            }
        }
    }
    uint8_t vailWatchCnt = vailWatch;
    if (vailWatch >= 2)
    {
        for (uint8_t j = 0; j < vailWatch; j++)
        {
            if (uteModuleWatchOnlineData.multipleWatchIndexBuff[j] > index)
            {
                vailWatchCnt--;
            }
        }
        vailWatch = vailWatchCnt;
    }
    //    uteModuleWatchOnlineData.multipleValidWatchCnt = vailWatch;

    //  if (index >= UTE_MODULE_SCREENS_WATCH_CNT_MAX)
    //  {
    //    return uteModuleWatchOnlineData.multipleWatchIndexBuff[index-UTE_MODULE_SCREENS_WATCH_CNT_MAX];
    //  }else{
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,vailWatch=%d\r\n", __func__, vailWatch);
    return vailWatch;
    //  }
}
#endif

/**
 *@brief        跳转默认表盘
 *@details     增加判断条件
 *@return ture 为需要刷新为默认表盘
 *@author       zn.zeng
 *@date       2022-06-28
 */
bool uteModuleWatchOnlineUpddateDefaultWatchIndex(void)
{
    uint8_t watchIndex = 0;
    bool isDefault = false;
    uteModuleGuiCommonGetCurrWatchIndex(&watchIndex);
    if ((watchIndex - UTE_MODULE_SCREENS_WATCH_CNT_MAX) == uteModuleWatchOnlineData.writeWatchIndex)
    {
        printf("qwqwqwqwwwwwwwwwwwwwwwwwww\r\n");
        uteModuleGuiCommonSetCurrWatchIndex(DEFAULT_WATCH_INDEX);
        isDefault = true;
    }
    return isDefault;
}
/**
 *@brief        通过索引获取表盘信息
 *@details
 *@param[in] uint8_t index,索引
 *@param[out] uint8_t *data,
 *@author       zn.zeng
 *@date       2022-07-05
 */
void uteModuleWatchOnlineGetInfoWithIndex(uint8_t index, uint8_t *data)
{
    if (index < UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        int watchConfigSize = sizeof(watchConfig_t);
        watchConfig_t watchConfig;
        uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[index], watchConfigSize);
        data[0] = (watchConfig.snNo >> 24) & 0xff;
        data[1] = (watchConfig.snNo >> 16) & 0xff;
        data[2] = (watchConfig.snNo >> 8) & 0xff;
        data[3] = watchConfig.snNo & 0xff;
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,error the index=%d is too large\r\n", __func__, index);
    }
}
/**
 *@brief        设置将要更新的表盘数据序号
 *@details
 *@param[in] uint8_t index,索引
 *@author       zn.zeng
 *@date       2022-07-05
 */
void uteModuleWatchOnlineSetWillUpdateDataIndex(uint8_t index)
{
    if (index < UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uteModuleWatchOnlineData.writeWatchIndex = index;
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,error the index=%d is too large\r\n", __func__, index);
    }
}

void uteModuleWatchOnlineWaitingDellCallback(void *pxTimer)
{
    uint8_t watchIndex = 0;
    uteModuleGuiCommonGetCurrWatchIndex(&watchIndex);
    // 等待当前表盘不是要同步的表盘后再擦除flash
    if (uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_WATCHMAIN_ID && (watchIndex - UTE_MODULE_SCREENS_WATCH_CNT_MAX) == uteModuleWatchOnlineData.writeWatchIndex)
    {
        uteModulePlatformRestartTimer(&uteModuleWatchOnlineWaitingDellTimerPointer, 100);
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,waiting to switch dials...", __func__);
    }
    else
    {
        uteModulePlatformFlashNorErase(uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex]);
        uteModuleWatchOnlineUpateConfigFromFlash();
        if (uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_WATCHMAIN_ID)
        {
            uteModuleGuiCommonSetCurrWatchIndex(DEFAULT_WATCH_INDEX);
            func_clock_recreate_dial();
            if(sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = true;
            }
            reset_sleep_delay_all();
        }
        uteModulePlatformStopTimer(&uteModuleWatchOnlineWaitingDellTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineWaitingDellTimerPointer);
        uteModuleWatchOnlineWaitingDellTimerPointer = NULL;
    }
}

/**
*@brief        删除表盘数据序号
*@details
*@param[in] uint8_t index,索引
*@author       wuhuowang
*@date       2023-02-09
*/
void uteModuleWatchOnlineDeleteDataIndex(uint8_t index, uint8_t *data)
{
    uint8_t watchIndex = 0;
    uint8_t multipleValidWatchCnt = 0;
    int watchConfigSize = sizeof(watchConfig_t);

    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,index=%d %d", __func__, index, uteModuleWatchOnlineData.multipleValidWatchCnt);

    if (index < UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uteModulePlatformFlashNorRead((uint8_t *)&uteModuleWatchOnlineData.watchConfig, uteModuleWatchOnlineMultipleBaseAddress[index], watchConfigSize);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,index error, index=%d max=%d", __func__, index, UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT);
        goto DELL_FAIL;
    }

    multipleValidWatchCnt = uteModuleWatchOnlineData.multipleValidWatchCnt;
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,multipleValidWatchCnt=%d", __func__, multipleValidWatchCnt);
    data[0] = UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT;

    if (multipleValidWatchCnt)
    {
        uteModuleWatchOnlineData.writeWatchIndex = index;
        uteModuleGuiCommonGetCurrWatchIndex(&watchIndex);
        if(uteModuleWatchOnlineWaitingDellTimerPointer == NULL)
        {
            uteModulePlatformCreateTimer(&uteModuleWatchOnlineWaitingDellTimerPointer, "WatchOnlineWaitingDellTimer", 1, 100, false, uteModuleWatchOnlineWaitingDellCallback);
        }
        if(uteModuleWatchOnlineWaitingDellTimerPointer != NULL)
        {
            if (uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_WATCHMAIN_ID && (watchIndex - UTE_MODULE_SCREENS_WATCH_CNT_MAX) == uteModuleWatchOnlineData.writeWatchIndex)
            {
                uteModuleGuiCommonSetCurrWatchIndex(DEFAULT_WATCH_INDEX);
                func_clock_recreate_dial();
                if(sys_cb.gui_sleep_sta)
                {
                    sys_cb.gui_need_wakeup = true;
                }
                reset_sleep_delay_all();
            }
            uteModulePlatformRestartTimer(&uteModuleWatchOnlineWaitingDellTimerPointer,100);
            data[1] = 0x01;
            data[2] = 0xFF;
            data[3] = 0xFF;
            data[4] = 0xFF;
            data[5] = 0xFF;
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,timer creation failed, watch online fail.", __func__);
            goto DELL_FAIL;
        }
    }
    else
    {
    DELL_FAIL:
        data[1] = 0x02;
        data[2] = 0xFF;
        data[3] = 0xFF;
        data[4] = 0xFF;
        data[5] = 0xFF;
    }
}

/**
 * @brief        一次删除多个表盘
 * @details
 * @param[in]    count 删除的表盘个数
 * @param[in]    *index 删除的表盘索引
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-06-29
 */
void uteModuleWatchOnlineDeleteDataMultipleIndex(uint8_t count, uint8_t *index)
{
    uint8_t *idx = NULL;
    idx = (uint8_t *)uteModulePlatformMemoryAlloc(count);
    memcpy(idx, index, count);

    uint8_t response[20];
    memset(response, 0x00, 20);
    response[0] = CMD_WATCH_ONLINE;
    response[1] = 0x0c;
    response[2] = count;

    if (uteModuleWatchOnlineData.multipleValidWatchCnt)
    {
        watchConfig_t watchConfig;
        uint8_t watchIndex = 0;
        uteModuleGuiCommonGetCurrWatchIndex(&watchIndex);
        for (uint8_t i = 0; i < count; i++)
        {
            if (idx[i] < UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
            {
                uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[idx[i]], sizeof(watchConfig_t));
                if (watchConfig.isWatchVaild == 0)
                {
                    if (uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_WATCHMAIN_ID && (watchIndex - UTE_MODULE_SCREENS_WATCH_CNT_MAX) == idx[i])
                    {
                        if (uteModuleWatchOnlineWaitingDellTimerPointer == NULL)
                        {
                            uteModulePlatformCreateTimer(&uteModuleWatchOnlineWaitingDellTimerPointer, "WatchOnlineWaitingDellTimer", 1, 100, false, uteModuleWatchOnlineWaitingDellCallback);
                        }
                        if (uteModuleWatchOnlineWaitingDellTimerPointer != NULL)
                        {
                            uteModuleWatchOnlineData.writeWatchIndex = idx[i];
                            uteModuleGuiCommonSetCurrWatchIndex(DEFAULT_WATCH_INDEX);
                            uteModulePlatformRestartTimer(&uteModuleWatchOnlineWaitingDellTimerPointer, 100);
                        }
                    }
                    else
                    {
                        uteModulePlatformFlashNorErase(uteModuleWatchOnlineMultipleBaseAddress[idx[i]]);
                    }
                    if (uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_WATCHMAIN_ID)
                    {
                        func_clock_recreate_dial();
                        if (sys_cb.gui_sleep_sta)
                        {
                            sys_cb.gui_need_wakeup = true;
                        }
                        reset_sleep_delay_all();
                    }
                    response[3 + i] = 1;
                }
                else
                {
                    response[3 + i] = 2;
                }
            }
        }
        uteModuleWatchOnlineUpateConfigFromFlash();
    }
    uteModuleProfileBleSendToPhone(response, 3 + count);

    uteModulePlatformMemoryFree(idx);
}

/**
 * @brief        发送所有表盘信息
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-05-24
 */
void uteModuleWatchOnlineGetAllInfo(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t response[20];
    memset(response, 0x00, 20);
    uint8_t sendSize = 3;
    int watchConfigSize = sizeof(watchConfig_t);
    watchConfig_t watchConfig;
    response[0] = CMD_WATCH_ONLINE;
    response[1] = 0x09;
    response[2] = sendParam->currSendFileIndex;
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,multipleValidWatchCnt=%d,currSendOffset=%d", __func__, uteModuleWatchOnlineData.multipleValidWatchCnt, sendParam->currSendFileDataOffset);
    if (sendParam->currSendFileDataOffset < UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uint8_t remaining = UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT - sendParam->currSendFileDataOffset;
        uint8_t maxCount = MIN(remaining, 3); // 最多发 3 个，也不能超过剩余数
        uint8_t vaildIndex = 0;
        for (uint8_t i = 0; i < maxCount; i++)
        {
            uint8_t index = sendParam->currSendFileDataOffset + i;
            uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[index], watchConfigSize);
            printf("%s,index=%d,snNo=%d\n", __func__, index, watchConfig.snNo);
            if (watchConfig.isWatchVaild == 0)
            {
                response[3 + vaildIndex * 5 + 0] = index;
                response[3 + vaildIndex * 5 + 1] = (watchConfig.snNo >> 24) & 0xff;
                response[3 + vaildIndex * 5 + 2] = (watchConfig.snNo >> 16) & 0xff;
                response[3 + vaildIndex * 5 + 3] = (watchConfig.snNo >> 8) & 0xff;
                response[3 + vaildIndex * 5 + 4] = watchConfig.snNo & 0xff;
                sendSize += 5;
                vaildIndex++;
            }
        }
        sendParam->currSendFileDataOffset += maxCount;
    }
    else
    {
        response[2] = 0xfd;
        uteApplicationCommonSyncDataTimerStop();
    }
    sendParam->currSendFileIndex++;
    uteModuleProfileBleSendToPhone(response, sendSize);
}

/**
 * @brief        切换当前表盘
 * @details
 * @param[in]    watchIndex 在线表盘index
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-07-10
 */
void uteModuleWatchOnlineSwitchWatchMain(uint8_t watchIndex, uint32_t watchId)
{
    uint8_t response[3];
    response[0] = CMD_WATCH_ONLINE;
    response[1] = 0x0d;
    response[2] = 0xff;

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,watchIndex=%d,watchId=%d",__func__,watchIndex,watchId);

    for (uint8_t i = UTE_MODULE_SCREENS_WATCH_CNT_MAX; i < uteModuleGuiCommonGetCurrWatchMaxIndex(); i++)
    {
        uint32_t address = func_clock_get_dialplate_info(i);
        if (address)
        {
            watchConfig_t watchConfig;
            uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, address, sizeof(watchConfig_t));
            if (watchConfig.isWatchVaild == 0 && watchId == watchConfig.snNo)
            {
                response[2] = 0x00;
                uteModuleGuiCommonSetCurrWatchIndex(i);
                if (uteModuleGuiCommonGetCurrentScreenId() != UTE_MOUDLE_SCREENS_WATCHMAIN_ID)
                {
                    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
                }
                else
                {
                    func_clock_recreate_dial();
                    if (sys_cb.gui_sleep_sta)
                    {
                        sys_cb.gui_need_wakeup = true;
                    }
                    reset_sleep_delay_all();
                }
                break;
            }
        }
    }

    uteModuleProfileBleSendToPhone(response, 3);
}

void uteModuleWatchOnlineGetAllInfoStart(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->currSendFileIndex = 0;
    param->currSendFileDataOffset = 0;
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleWatchOnlineGetAllInfo);
    uteApplicationCommonSyncDataTimerStart();
}

#if UTE_MODULE_WATCH_PHOTO_SUPPORT
/**
 * @brief        获取相册表盘一张图片最大大小
 * @details      相册表盘占用一个在线表盘位，为一个在线表盘位的最大空间，如果支持多张图片，则每张图片平分此空间
 * @return       获取成功返回图片最大大小Byte
 * @author       Wang.Luo
 * @date         2025-05-30
 */
uint32_t uteModuleWatchOnlineGetOnePhotoMaxSize(void)
{
    uint32_t onePhotoMaxSize = UTE_MODULE_WATCHONLINE_MAX_SIZE / UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT - UTE_MODULE_WATCH_PHOTO_BASSADDR_OFFSET;
    onePhotoMaxSize = onePhotoMaxSize / UTE_MODULE_WATCH_PHOTO_MAX_PICTURE_CNT;
    onePhotoMaxSize = (onePhotoMaxSize / 4096) * 4096;
    return onePhotoMaxSize;
}

/**
 * @brief        更新相册表盘信息
 * @details      初始化、同步、删除在线表盘后调用
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-06-03
 */
void uteModuleWatchOnlineUpdatePhotoWatchInfo(void)
{
    watchConfig_t watchConfig;
    uteModuleWatchOnlineData.photoWatchIndex = 0;
    uteModuleWatchOnlineData.isHasPhoto = false;
    uteModuleWatchOnlineData.photoWatchPictureCnt = 0;
    uteModuleWatchOnlineData.isStartReceivePhoto = false;
    for (uint8_t i = 0; i < UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT; i++)
    {
        uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[i], sizeof(watchConfig_t));
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL,"%s,watchConfig.snNo=%x,isWatchVaild=%d",__func__,watchConfig.snNo,watchConfig.isWatchVaild);
        if (watchConfig.snNo == UTE_MODULE_WATCH_PHOTO_DEFAULT_ID && watchConfig.isWatchVaild == 0)
        {
            uteModuleWatchOnlineData.photoWatchIndex = i;
            uteModuleWatchOnlineData.isHasPhoto = true;
            break;
        }
    }
    if (uteModuleWatchOnlineData.isHasPhoto)
    {
        uint32_t bassAddr = uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.photoWatchIndex] + UTE_MODULE_WATCH_PHOTO_BASSADDR_OFFSET;
        uint32_t onePhotoMaxSize = uteModuleWatchOnlineGetOnePhotoMaxSize();
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,bassAddr:0x%x", __func__, bassAddr);
        for (uint8_t i = 0; i < UTE_MODULE_WATCH_PHOTO_MAX_PICTURE_CNT; i++)
        {
            uteModulePlatformFlashNorRead((uint8_t *)&uteModuleWatchOnlineData.photoWatchConfig[i], bassAddr + i * onePhotoMaxSize, sizeof(photoWatchConfig_t));
            if (uteModuleWatchOnlineData.photoWatchConfig[i].isVaild == 0)
            {
                uint32_t bassAddr = 0;
                uint32_t preview = 0;
                uint32_t photo = 0;
                uteModuleWatchOnlineGetPhotoAddress(i, &bassAddr, &preview, &photo);
                area_t img_area;
                img_area = gui_image_get_size(photo);
                if (img_area.wid > 0 && img_area.hei > 0)
                {
                    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL,"%s,img_area wid=%d,hei=%d",__func__,img_area.wid,img_area.hei);
                    uteModuleWatchOnlineData.photoWatchPictureCnt++;
                }
            }
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,addr:0x%x,isValid:%d", __func__, bassAddr + i * onePhotoMaxSize, uteModuleWatchOnlineData.photoWatchConfig[i].isVaild);
        }
        if(uteModuleWatchOnlineData.photoWatchPictureCnt == 0)
        {
            uteModuleWatchOnlineData.isHasPhoto = false;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,isHasPhoto=%d,photoWatchIndex=%d", __func__, uteModuleWatchOnlineData.isHasPhoto, uteModuleWatchOnlineData.photoWatchIndex);
}

/**
 * @brief        判断是否有相册表盘
 * @details
 * @return       bool 存在返回true 否则返回false
 * @author       Wang.Luo
 * @date         2025-06-04
 */
bool uteModuleWatchOnlineIsHasPhoto(void)
{
    return uteModuleWatchOnlineData.isHasPhoto;
}

/**
 * @brief        获取相册表盘指定图片地址
 * @details
 * @param[in]    index 图片索引
 * @param[out]   preview 预览图地址
 * @param[out]   photo 图片地址
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-06-04
 */
void uteModuleWatchOnlineGetPhotoAddress(uint8_t index, uint32_t *bassAddr, uint32_t *preview, uint32_t *photo)
{
    // if (index >= uteModuleWatchOnlineData.photoWatchPictureCnt || uteModuleWatchOnlineData.photoWatchConfig[index].isVaild)
    // {
    //     UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,index=%d,photoWatchConfig[index].isVaild=%d", __func__, index, uteModuleWatchOnlineData.photoWatchConfig[index].isVaild);
    //     return false;
    // }
    uint32_t photoBassAddr = uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.photoWatchIndex] + UTE_MODULE_WATCH_PHOTO_BASSADDR_OFFSET;
    *bassAddr = photoBassAddr + index * uteModuleWatchOnlineGetOnePhotoMaxSize();
    *preview = *bassAddr + sizeof(photoWatchConfig_t);
    *photo = uteModuleWatchOnlineData.photoWatchConfig[index].previewSize > 0 ? *preview + uteModuleWatchOnlineData.photoWatchConfig[index].previewSize : 0;
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,photoWatchIndex:%d,picIndex:%d,photoBassAddr=0x%x,preview=0x%x,photo=0x%x", __func__, uteModuleWatchOnlineData.photoWatchIndex, index, *bassAddr, *preview, *photo);
}

void uteModuleWatchOnlineGetCurrPhotoAddress(uint32_t *preview, uint32_t *photo)
{
    if (uteModuleWatchOnlineData.photoWatchPictureCnt)
    {
        if (uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].isVaild == 0)
        {
            for (uint8_t i = 0; i < UTE_MODULE_WATCH_PHOTO_MAX_PICTURE_CNT; i++)
            {
                if (uteModuleWatchOnlineData.photoWatchConfig[i].isVaild == 0)
                {
                    uteModuleWatchOnlineData.photoWatchPictureIndex = i;
                    break;
                }
            }
        }
        uint32_t photoBassAddr = uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.photoWatchIndex] + UTE_MODULE_WATCH_PHOTO_BASSADDR_OFFSET;
        photoBassAddr = photoBassAddr + uteModuleWatchOnlineData.photoWatchPictureIndex * uteModuleWatchOnlineGetOnePhotoMaxSize();
        *preview = photoBassAddr + sizeof(photoWatchConfig_t);
        *photo = *preview + uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].previewSize;
    }
    else
    {
        *preview = 0;
        *photo = 0;
    }

    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,photoWatchIndex:%d,picIndex:%d,preview=0x%x,photo=0x%x", __func__, uteModuleWatchOnlineData.photoWatchIndex, uteModuleWatchOnlineData.photoWatchPictureIndex, *preview, *photo);
}

void uteModuleWatchOnlineGetCurrPhotoWatchConfig(photoWatchConfig_t *config)
{
    memcpy(config, &uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex], sizeof(photoWatchConfig_t));
}

/**
 * @brief        获取相册表盘信息
 * @details
 * @param[out]   data 响应数据
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-06-04
 */
void uteModuleWatchOnlineGetInfoWithPhoto(uint8_t *data)
{
    uint32_t onePhotoMaxSize = uteModuleWatchOnlineGetOnePhotoMaxSize() - sizeof(photoWatchConfig_t);
    uint16_t previewWidth = (UTE_DRV_SCREEN_WIDTH / 2.4f + 0.5);
    uint16_t previewHeight = (UTE_DRV_SCREEN_HEIGHT / 2.4f + 0.5);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,onePhotoMaxSize:%d,previewWidth:%d,previewHeight:%d", __func__, onePhotoMaxSize, previewWidth, previewHeight);
    data[2] = UTE_MODULE_WATCH_PHOTO_MAX_PICTURE_CNT;
    data[3] = uteModuleWatchOnlineData.photoWatchPictureCnt;
    data[4] = uteModuleWatchOnlineData.photoWatchPictureIndex;
    data[5] = 1;
    data[6] = UTE_MODULE_WATCH_PHOTO_CUSTOM_FONT_COLOR_SUPPORT;
    data[7] = 0;
    data[8] = 0;
    data[9] = 0;
    data[10] = previewWidth >> 16 & 0xff;
    data[11] = previewWidth & 0xff;
    data[12] = previewHeight >> 16 & 0xff;
    data[13] = previewHeight & 0xff;
    data[14] = onePhotoMaxSize >> 24 & 0xff;
    data[15] = onePhotoMaxSize >> 16 & 0xff;
    data[16] = onePhotoMaxSize >> 8 & 0xff;
    data[17] = onePhotoMaxSize & 0xff;
}

void uteModuleWatchOnlineStartSyncPhoto(uint8_t *data)
{
    uint32_t previewSize = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
    uint32_t bgSize = (data[12] << 24) | (data[13] << 16) | (data[14] << 8) | data[15];
    uteModuleWatchOnlineData.ReceivePhotoHead.fileSize = previewSize + bgSize;
    uteModuleWatchOnlineData.ReceivePhotoHead.fileCrc = (data[16] << 24) | (data[17] << 16) | (data[18] << 8) | data[19];
    uteModuleWatchOnlineData.ReceivePhotoHead.snNo = UTE_MODULE_WATCH_PHOTO_DEFAULT_ID;
    uteModuleWatchOnlineData.ReceivePhotoHead.isWatchVaild = 0;
    uint32_t onePhotoMaxSize = uteModuleWatchOnlineGetOnePhotoMaxSize() - sizeof(photoWatchConfig_t);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,ReceivePhotoHead.fileSize:%d,onePhotoMaxSize:%d", __func__, uteModuleWatchOnlineData.ReceivePhotoHead.fileSize, onePhotoMaxSize);
    if (uteModuleWatchOnlineData.ReceivePhotoHead.fileSize > onePhotoMaxSize)
    {
        uint8_t response[3];
        memset(response, 0x00, sizeof(response));
        response[0] = CMD_WATCH_ONLINE;
        response[1] = 0x0b;
        response[2] = 0x00;
        response[2] = 0x01;
        uteModuleProfileBleSendToPhone(response, 3);
    }
    else
    {
        uteModuleWatchOnlineData.isStartReceivePhoto = true;
        uteModuleWatchOnlineData.photoWatchPictureIndex = data[2] >= UTE_MODULE_WATCH_PHOTO_MAX_PICTURE_CNT ? 0 : data[2];
        uteModuleWatchOnlineData.photoWatchIndex = uteModuleWatchOnlineData.writeWatchIndex;
        uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].bgSize = bgSize;
        uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].previewSize = previewSize;
        uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].isVaild = 0;
        uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].timePosition = data[4];
        uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].fontColor = data[5] << 16 | data[6] << 8 | data[7];
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,photoWatchPictureIndex:%d,bgSize:%d,previewSize:%d,timePosition:%d,fontColor:0x%x", __func__, uteModuleWatchOnlineData.photoWatchPictureIndex, bgSize, previewSize, uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].timePosition, uteModuleWatchOnlineData.photoWatchConfig[uteModuleWatchOnlineData.photoWatchPictureIndex].fontColor);
        uteModuleWatchOnlineReadyStart();
    }
}

void uteModuleWatchOnlineSetStartReceivePhoto(bool isStart)
{
    uteModuleWatchOnlineData.isStartReceivePhoto = isStart;
}

#endif

#endif //UTE_MODULE_WATCHONLINE_SUPPORT

