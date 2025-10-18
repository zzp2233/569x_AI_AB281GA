/**
*@file
*@brief        快捷回复处理模块
*@details
*@author       xjc
*@date         2022-07-01
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_quickReply.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_application_common.h"
#include "ute_module_charencode.h"
#include "ute_drv_motor.h"
#include "ute_module_call.h"
#include "ute_module_gui_common.h"
#include "func_cover.h"

#if UTE_MODUEL_QUICK_REPLY_SUPPORT
ute_quickReply_data_t uteModuleQuickReplyData;

/*互斥量*/
void *uteModuleQuickReplyMute;

/**
*@brief  初始化函数
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplyInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_QUICK_REPLY_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_QUICK_REPLY_DIR, NULL, NULL);
#endif
    uteModulePlatformCreateMutex(&uteModuleQuickReplyMute);
    memset(&uteModuleQuickReplyData, 0, sizeof(ute_quickReply_data_t));
    uteModuleQuickReplyReadConfig();
}

/**
*@brief  读取config
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplyReadConfig(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = 0;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_QUICK_REPLY_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleQuickReplyData.isOpen = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isOpen=%d", __func__,uteModuleQuickReplyData.isOpen);
}

/**
*@brief  保存config
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplySaveConfig(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleQuickReplyData.isOpen;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_QUICK_REPLY_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief  设置快捷回复开关的状态
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplySetStatus(bool isOpen)
{
    uteModuleQuickReplyData.isOpen = isOpen;
    uteModuleQuickReplySaveConfig();
}

/**
*@brief  获取快捷回复开关的状态
*@details
*@author        xjc
*@date        2022-07-05
*/
bool uteModuleQuickReplyGetStatus(void)
{
    ute_ble_connect_state_t connectStatus;
    uteApplicationCommonGetBleConnectionState(&connectStatus);
    if (uteModuleQuickReplyData.isOpen && connectStatus.isConnected)
    {
        return true;
    }
    return false;
}

/**
*@brief  删除所有快捷消息内容
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplyDelAllContentData(void)
{
    char path[20];
    memset(&path[0],0,sizeof(path));
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_QUICK_REPLY_DIR, dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    for (int i = 0; i < dirInfo->filesCnt; i++)
    {
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_QUICK_REPLY_DIR, dirInfo->filesName[i]);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,del file=%s", __func__,path);
        uteModuleFilesystemDelFile(&path[0]);
    }
    uteModuleQuickReplyData.totalContentCnt = 0;
    uteModulePlatformMemoryFree(dirInfo);
}

/**
*@brief  保存快捷消息内容
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplySaveContent(void)
{
    void *file;
    uint8_t buffSize = sizeof(ute_quickReply_receive_t);
    char path[20];
    memset(&path[0],0,sizeof(path));
    /*!删除已保存的内容*/
    if(uteModuleQuickReplyData.receiveData.currentIndex == 1)
    {
        uteModuleQuickReplyDelAllContentData();
    }
    sprintf((char *)&path[0],"%s/%02d",UTE_MODULE_FILESYSTEM_QUICK_REPLY_DIR,uteModuleQuickReplyData.receiveData.currentIndex);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,path=%s", __func__,&path[0]);
    /*!保存新的内容*/
    if(uteModuleFilesystemOpenFile(&path[0],&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,(void*)&uteModuleQuickReplyData.receiveData,buffSize);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief     读取所有快捷消息到内存
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplyReadContentData(void)
{
    void *file;
    uint16_t buffSize = sizeof(ute_quickReply_receive_t);
    char path[20];
    memset(&path[0],0,sizeof(path));
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,start rtc=%d", __func__,uteModulePlatformGetSystemTick());
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_QUICK_REPLY_DIR, dirInfo, NULL);
    uteModulePlatformTakeMutex(uteModuleQuickReplyMute);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    uint8_t vailCnt = 0;
    for (int i = 0; i < dirInfo->filesCnt; i++)
    {
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_QUICK_REPLY_DIR, dirInfo->filesName[i]);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,path=%s", __func__,path);
        if(uteModuleFilesystemOpenFile(&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,(void*)&uteModuleQuickReplyData.contentData[vailCnt],buffSize);
            uteModuleFilesystemCloseFile(file);
            vailCnt++;
        }
    }
    uteModuleQuickReplyData.totalContentCnt = vailCnt;
    uteModulePlatformGiveMutex(uteModuleQuickReplyMute);
    uteModulePlatformMemoryFree(dirInfo);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.totalContentCnt=%d,rtc=%d", __func__,uteModuleQuickReplyData.totalContentCnt,uteModulePlatformGetSystemTick());
}

/**
*@brief        保存快捷回复的电话号码
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplySetNumber(uint8_t*number,uint8_t length)
{
    uteModuleQuickReplyData.numberSize = length;
    memset(&uteModuleQuickReplyData.number[0],0,UTE_CALL_DIAL_NUMBERS_MAX);
    memcpy(&uteModuleQuickReplyData.number[0],number,uteModuleQuickReplyData.numberSize);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,length=%d", __func__,uteModuleQuickReplyData.numberSize);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL, &uteModuleQuickReplyData.number[0],uteModuleQuickReplyData.numberSize);
}

/**
*@brief        存在快捷回复的条数
*@details
*@author        xjc
*@date        2022-07-05
*/
uint8_t uteModuleQuickReplyGetTotalContentCnt(void)
{
    return uteModuleQuickReplyData.totalContentCnt;
}

/**
*@brief     获取快捷回复数据
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplyGetContentData(ute_quickReply_receive_t *data)
{
    uteModulePlatformTakeMutex(uteModuleQuickReplyMute);
    memcpy(data,&uteModuleQuickReplyData.contentData[0],sizeof(ute_quickReply_receive_t)*UTE_MODUEL_QUICK_REPLY_MAX_MSG_COUNT);
    uteModulePlatformGiveMutex(uteModuleQuickReplyMute);
}

/**
*@brief     快捷回复数据
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplySendContentData(uint8_t index)
{
    uint8_t response[3 + UTE_CALL_DIAL_NUMBERS_MAX + UTE_MODUEL_QUICK_REPLY_MSG_MAX_SIZE];
    memset(&response[0], 0, sizeof(response));
    response[0] = CMD_QUICK_REPLY_CTRL;
    //联系人
    response[1] = uteModuleQuickReplyData.numberSize;
    memcpy(&response[2],&uteModuleQuickReplyData.number[0],uteModuleQuickReplyData.numberSize);
    // content
    uint16_t uft8Len = 0;
    // uteModuleCharencodeUnicodeConversionUtf8(&uteModuleQuickReplyData.contentData[index].receiveContent[0], uteModuleQuickReplyData.contentData[index].receiveContentLen, &response[2+uteModuleQuickReplyData.numberSize+1], &uft8Len, UTE_MODUEL_QUICK_REPLY_MSG_MAX_SIZE);
    uft8Len = uteModuleQuickReplyData.contentData[index].receiveContentLen;
    memcpy(&response[2+uteModuleQuickReplyData.numberSize+1],&uteModuleQuickReplyData.contentData[index].receiveContent[0],uft8Len);
    response[2+uteModuleQuickReplyData.numberSize] = uft8Len;
    uteModuleProfileBle50SendToPhone(&response[0], 3 + uteModuleQuickReplyData.numberSize + uft8Len);
#if APP_QUICK_REPLY_DISPLAY_RESAULT_SCREEN_NOW
    ute_ble_connect_state_t connectStatus;
    uteApplicationCommonGetBleConnectionState(&connectStatus);
    if(connectStatus.isConnected)
    {
        uteModuleQuickReplySendSuccess();
    }
    else
    {
        uteModuleQuickReplySendFail();
    }
#endif
}

/**
*@brief        处理快捷回复协议
*@details
*@author        xjc
*@date        2022-07-05
*/
void uteModuleQuickReplyCmd(uint8_t*receive,uint8_t length)
{
    uint8_t response[20];
    memset(&response[0],0,sizeof(response));
    response[0] = receive[0];
    if(receive[0] == CMD_QUICK_REPLY_GET_PHONE_NUMBER) /*!短信或来电通知后，APP下发手机号码 0x45*/
    {
        if (receive[1] == 0xFA)
        {
            uteModuleQuickReplySetNumber(&receive[3],receive[2]);
            uteModuleProfileBle50SendToPhone(&receive[0], 2);
        }
    }
    else if(receive[0] == CMD_QUICK_REPLY_CONTENT) /*!自定义短信快捷回复内容功能 0x46*/
    {
        if (receive[1] == 0xFA)
        {
            if (receive[2] <= UTE_MODUEL_QUICK_REPLY_MAX_MSG_COUNT)
            {
                uteModuleQuickReplyData.receiveData.maxIndex = receive[2];
                uteModuleQuickReplyData.receiveData.currentIndex = receive[3]+1; /*!从1开始*/
                if(receive[4] <= UTE_MODUEL_QUICK_REPLY_MSG_MAX_SIZE)
                {
                    uteModuleQuickReplyData.receiveData.receiveContentLen = UTE_MODUEL_QUICK_REPLY_MSG_MAX_SIZE;
                    memset(&uteModuleQuickReplyData.receiveData.receiveContent[0],0,sizeof(uteModuleQuickReplyData.receiveData.receiveContent));
                    memcpy(&uteModuleQuickReplyData.receiveData.receiveContent[0],&receive[5],receive[4]);
                    uteModuleQuickReplyData.receiveData.receiveContentLen = receive[4];
                    // uteModuleCharencodeUtf8ConversionUnicode(&uteModuleQuickReplyData.receiveData.receiveContent[0],&uteModuleQuickReplyData.receiveData.receiveContentLen,&receive[5],receive[4]);
                    uteModuleQuickReplySaveContent();
                }
                else
                {
                    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,receive quick reply content failed, out of range!", __func__);
                }
            }
            response[1] = receive[1];
            response[2] = receive[2];
            response[3] = receive[3];
            uteModuleProfileBle50SendToPhone(&response[0], 4);
        }
        else if (receive[1] == 0xFD)
        {
            response[1] = receive[1];
            uteModuleProfileBle50SendToPhone(&response[0], 2);
            /*读取所有快捷消息到内存*/
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_QUICK_REPLY_READ_DATA,0);
        }
        else if (receive[1] == 0xFB)
        {
            /*!删除所有快捷消息内容*/
            uteModuleQuickReplyDelAllContentData();
            response[1] = 0xFB;
            uteModuleProfileBle50SendToPhone(&response[0], 2);
        }
    }
    else if(receive[0] == CMD_QUICK_REPLY_CTRL) /*!自定义短信快捷回复功能 0x52*/
    {
        if(length == 2)
        {
            switch (receive[1])
            {
                case 0x00: /*close*/
                    uteModuleQuickReplySetStatus(false);
                    response[1] = 0x00;
                    response[2] = 0xFD;
                    uteModuleProfileBle50SendToPhone(&response[0], 3);
                    break;
                case 0x01: /*open*/
                    uteModuleQuickReplySetStatus(true);
                    response[1] = 0x01;
                    response[2] = 0xFD;
                    uteModuleProfileBle50SendToPhone(&response[0], 3);
                    break;
                case 0xFE: /*send success*/
                    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,send quick reply success!", __func__);
                    response[1] = 0xFE;
                    uteModuleProfileBle50SendToPhone(&response[0], 2);
#if UTE_MODULE_SCREEN_QUICK_MESSAGE_SEND_RESAULT_SUPPORT && (!APP_QUICK_REPLY_DISPLAY_RESAULT_SCREEN_NOW)
                    uteModuleQuickReplySendSuccess();
#endif
                    break;
                case 0xFF: /*send failed*/
                    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,send quick reply failed!", __func__);
                    response[1] = 0xFF;
                    uteModuleProfileBle50SendToPhone(&response[0], 2);
#if UTE_MODULE_SCREEN_QUICK_MESSAGE_SEND_RESAULT_SUPPORT && (!APP_QUICK_REPLY_DISPLAY_RESAULT_SCREEN_NOW)
                    uteModuleQuickReplySendFail();
#endif
                    break;
                default:
                    break;
            }
        }
    }
}
#if UTE_MODULE_SCREEN_QUICK_MESSAGE_SEND_RESAULT_SUPPORT
/**
*@brief        快捷回复成功
*@details
*@author        黄鹤
*@date        2022-08-05
*/
void uteModuleQuickReplySendSuccess(void)
{
    uteModuleQuickReplyData.isSendSuccess = true;
#if UTE_MODULE_SCREEN_QUICK_MESSAGE_SEND_RESAULT_SUPPORT && (!APP_QUICK_REPLY_DISPLAY_RESAULT_SCREEN_NOW)
    sys_cb.cover_index = REMIND_COVER_SUCC;
    msgbox((char*)i18n[STR_QUICK_SUCC], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
    //sys_cb.remind_tag = true;
#endif
}
/**
*@brief        快捷回复失败
*@details
*@author        黄鹤
*@date        2022-08-05
*/
void uteModuleQuickReplySendFail(void)
{
    uteModuleQuickReplyData.isSendSuccess = false;
#if UTE_MODULE_SCREEN_QUICK_MESSAGE_SEND_RESAULT_SUPPORT && (!APP_QUICK_REPLY_DISPLAY_RESAULT_SCREEN_NOW)
    sys_cb.cover_index = REMIND_COVER_FAIL;
    //sys_cb.remind_tag = true;
    msgbox((char*)i18n[STR_QUICK_FAIL], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
#endif
}
/**
*@brief        获取快捷回复是否成功
*@details
*@return isSuccess 是否成功
*@author        黄鹤
*@date        2022-08-05
*/
bool uteModuleQuickReplyGetIsSendSuccess(void)
{
    return uteModuleQuickReplyData.isSendSuccess;
}
#endif
#endif

