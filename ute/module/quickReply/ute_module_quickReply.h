/**
*@file
*@brief        快捷回复处理模块
*@details
*@author       xjc
*@date         2022-07-01
*@version      v1.0
*/
#ifndef _UTE_MODULE_QUICK_REPLY_H_
#define _UTE_MODULE_QUICK_REPLY_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_call.h"
#include "include.h"

#if UTE_MODUEL_QUICK_REPLY_SUPPORT

/*! 快捷信息数据保存目录,只保存具体的消息内容 xjc, 2022-07-05  */
#define UTE_MODULE_FILESYSTEM_QUICK_REPLY_DIR "quickReply"
/*! 保存快捷信息开关参数 xjc, 2022-07-05  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_QUICK_REPLY_INFO "systemparam/quickReplyInfo"
/*! 快捷信息最大保存条数 xjc, 2022-07-05  */
#define UTE_MODUEL_QUICK_REPLY_MAX_MSG_COUNT 8
/*! 每条快捷信息最大长度 xjc, 2022-07-05  */
#define UTE_MODUEL_QUICK_REPLY_MSG_MAX_SIZE 120

typedef struct
{
    uint8_t currentIndex;
    uint8_t maxIndex;
    uint16_t receiveContentLen;
    uint8_t receiveContent[UTE_MODUEL_QUICK_REPLY_MSG_MAX_SIZE];
} ute_quickReply_receive_t;

typedef struct
{
    bool isOpen; /*开启状态，是否打开快捷回复功能*/
    uint8_t number[UTE_CALL_DIAL_NUMBERS_MAX];
    uint8_t numberSize;
    ute_quickReply_receive_t receiveData;
    uint8_t totalContentCnt;
    ute_quickReply_receive_t contentData[UTE_MODUEL_QUICK_REPLY_MAX_MSG_COUNT];
#if UTE_MODULE_SCREEN_QUICK_MESSAGE_SEND_RESAULT_SUPPORT
    bool isSendSuccess; /* 快捷回复是否成功 */
#endif
} ute_quickReply_data_t;

void uteModuleQuickReplyInit(void);
void uteModuleQuickReplyReadConfig(void);
void uteModuleQuickReplySetStatus(bool isOpen);
bool uteModuleQuickReplyGetStatus(void);
void uteModuleQuickReplyReadContentData(void);
void uteModuleQuickReplySetNumber(uint8_t*number,uint8_t length);
uint8_t uteModuleQuickReplyGetTotalContentCnt(void);
void uteModuleQuickReplyGetContentData(ute_quickReply_receive_t *data);
void uteModuleQuickReplySendContentData(uint8_t index);
void uteModuleQuickReplyCmd(uint8_t*receive,uint8_t length);
#if UTE_MODULE_SCREEN_QUICK_MESSAGE_SEND_RESAULT_SUPPORT
void uteModuleQuickReplySendSuccess(void);
void uteModuleQuickReplySendFail(void);
bool uteModuleQuickReplyGetIsSendSuccess(void);
#endif
#endif
#endif

